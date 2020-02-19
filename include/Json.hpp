#pragma once
#include <map>
#include <typeinfo>
#include <functional>
#include <string>
#include <vector>
#include <array>
#include <any>
#include <cassert>

using jstring = std::string;

class JsonArray {
protected:
    static std::vector<jstring> jsonArrayParse(jstring jsn);
public:
    virtual void load(jstring jsnArray) = 0;
    virtual jstring stringify() const = 0;
};

template<typename T>
class JVector : public JsonArray, public std::vector<T> {
public:
    void load(jstring jsnArray) override;
    jstring stringify() const override;
    operator std::vector<T>& () { return *this; }
};

template<typename T, size_t N>
class JArray : public JsonArray, public std::array<T, N> {
public:
    void load(jstring jsnArray) override;
    jstring stringify() const override;
    operator std::array<T, N>& () { return *this; }
};

class Json {

template<typename T1, typename T2>
using jmap = std::map<T1, T2>;

protected:
    jmap<std::string, std::pair<std::string, void*>> _json_map;
    static jmap<std::string, std::function<void(void *, std::string)>> _json_assign;
    static jmap<std::string, std::function<void(void *, void *)>> _json_operator_eq;
    static jmap<std::string, std::function<jstring(void *)>> _json_stringify;

    template<typename T>
    T _JsnVar(std::string name, T *var, T val = T()) {
        std::string varType = typeid(T).name();
        if constexpr (std::is_base_of_v<JsonArray, T>) {
            varType = "Array";
        } else if constexpr (std::is_base_of_v<Json, T>) {
            varType = "Object";
        } else if (!_json_assign.count(varType)) {
            throw "invalide type"; //TODO error
        }
        _json_map[name] = {varType, var};
        return val;
    }

public:
    Json() = default;

    virtual void load(jstring jsn) final;
    virtual Json& operator = (const Json& assign) {
        for (auto& [name, pair] : _json_map) {
            _json_operator_eq[pair.first](pair.second, assign._json_map.at(name).second);
        }
        return *this;
    }
    virtual Json& operator = (const Json&& assign) {
        for (auto& [name, pair] : _json_map) {
            _json_operator_eq[pair.first](pair.second, assign._json_map.at(name).second);
        }
        return *this;
    }

    template<typename T>
    static T* create(jstring jsn) {
        // TODO return my safe_ptr
        T* ptr_val = new T;
        T& val = *ptr_val;
        if constexpr (std::is_base_of_v<Json, T> || std::is_base_of_v<JsonArray, T>) {
            val.load(jsn);
            return ptr_val;
        } else if (_json_assign.count(typeid(T).name())) {
            _json_assign.at(typeid(T).name())((void *)ptr_val, jsn);
            return ptr_val;
        }
        // ERROR
        return nullptr;
    }

    template<typename T>
    static jstring serialize(const T val) {
        if constexpr (std::is_base_of_v<Json, T> || std::is_base_of_v<JsonArray, T>) {
            return val.stringify();
        } else if (_json_assign.count(typeid(T).name())) {
            return _json_stringify.at(typeid(T).name())((void *)(&val));
        }
        // ERROR
        return "";
    }
    jstring stringify(short flags = 0) const;
};

#define VARNAME(name) m_##name

#define _JVAR(name, type) type VARNAME(name) = _JsnVar(#name, &VARNAME(name)

#define JVar(name, type) _JVAR(name, type))

#define JVarVal(name, type, val) _JVAR(name, type), val)

#define JCtorDefault(type) type () = default;\
type (const type & cpy) { Json::operator=(cpy); }\
type (const type && cpy) { Json::operator=(cpy); }

#define JsonInit(cpy) Json::operator=(cpy)

template<typename T>
void JVector<T>::load(jstring jsnArray) {
    auto vals = jsonArrayParse(jsnArray);
    for (auto& jval : vals) {
        auto* ptr = Json::create<T>(jval);
        this->push_back(*ptr);
        delete ptr;
    }
}

template<typename T>
jstring JVector<T>::stringify() const {
    std::string jsn{"["};
    for (auto& elem : *this) {
        jsn += Json::serialize(elem) + ", ";
    }
    jsn.pop_back();
    jsn.back() = ']';
    return jsn;
}

template<typename T, size_t N>
void JArray<T, N>::load(jstring jsnArray) {
    auto vals = jsonArrayParse(jsnArray);
    for (size_t i = 0; i < N && i < vals.size(); ++i) {
        auto *ptr = Json::create<T>(vals[i]);
        (*this)[i] = *ptr;
        delete ptr;
    }
}

template<typename T, size_t N>
jstring JArray<T, N>::stringify() const {
    std::string jsn{"["};
    for (auto& elem : *this) {
        jsn += Json::serialize(elem) + ", ";
    }
    jsn.pop_back();
    jsn.back() = ']';
    return jsn;
}