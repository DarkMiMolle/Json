#pragma once
#include "Json.hpp"
//#include <stdlib.h>

class JsnAny final : public Json {
private:
    static void *new_val(std::string type, void *val);
    static jstring any_stringify(std::string type, void *val);
    struct value {
        void *ptr = nullptr;
        std::string type = "";
    } unique_val;
    struct JAnyArray final : public JsonArray {
        std::map<void *, std::string> array;
        void load(jstring jsnArray) override;
        jstring stringify() const override;
    };
public:
    JsnAny() = default;
    JsnAny(const Json& json);
    JsnAny(const Json&& json);
    JsnAny(const JsonArray& json);
    JsnAny(const JsonArray&& json);
    template<typename T, std::enable_if_t<!std::is_base_of_v<Json, T>, int> = 0>
    JsnAny(T val);

    template<typename T>
    T* operator[] (std::string att) noexcept;

    template<typename T>
    T& at(std::string att);

    template<typename T>
    operator T* ();

    template<typename T>
    T& get();

    template<typename T>
    T as() const;

    template<typename T>
    JsnAny& operator=(const T& assign);

    template<typename T>
    JsnAny& operator=(const T&& assign);

    void load(jstring jsn) override;

    ~JsnAny();
};

template<typename T, std::enable_if_t<!std::is_base_of_v<Json, T>, int>>
JsnAny::JsnAny(T val) {
    unique_val = {new_val(typeid(T).name(), &val), typeid(T).name()};
}

template<typename T>
T* JsnAny::operator[] (std::string att) noexcept {
    if (unique_val.ptr) return nullptr;
    if (!_json_map.count(att)) return nullptr; // TODO: think what i do here
    return (T *)_json_map[att].second;
}

template<typename T>
T& JsnAny::at(std::string att) {
    return *(T *)(_json_map.at(att).first);
}

template<typename T>
T JsnAny::as() const {
    if (!unique_val.ptr) {
        auto* created = Json::create<T>(this->stringify());
        T ret(*created);
        return ret;
    }
    return *static_cast<T*>(unique_val.ptr);
}

template<typename T>
JsnAny& JsnAny::operator=(const T& assign) {
    if (unique_val.ptr) {
        free(unique_val.ptr);
        unique_val = {nullptr, ""};
    }
    _json_map.clear();
    if constexpr (std::is_base_of_v<Json, T>) {
        if constexpr (std::is_same_v<JsnAny, T>) {
            if (assign.unique_val.ptr) {
                unique_val = {new_var(assign.unique_val.type, assign.unique_val.ptr), assign.unique_val.type};
                return *this;
            }
        }
        for (auto& [name, var] : assign._json_map) {
            _json_map[name] = {var.first, new_val(var.first, var.second)};
        }
        return *this;
    } else if constexpr (std::is_base_of_v<JsonArray, T>) {
        return *this;
    } else if (_json_assign.count(typeid(T).name())) {

    }
    throw "invalide type"; // TODO
}

template<typename T>
JsnAny& JsnAny::operator=(const T&& assign) {
    if (unique_val.ptr) {
        free(unique_val.ptr);
        unique_val.ptr = nullptr;
    }
    if constexpr (std::is_same_v<T, JsnAny>) {

    } else if constexpr (std::is_base_of_v<Json, T> || std::is_base_of_v<JsonArray, T>) {
        this->load(assign.stringify());
        return *this;
    } else if (_json_assign.count(typeid(T).name())) {
        _json_map.clear();
        auto v = assign;
        unique_val = {new_val(typeid(T).name(), &v), typeid(T).name()};

        return *this;
    }
    throw "invalide type"; // TODO
}