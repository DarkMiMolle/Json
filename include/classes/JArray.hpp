#pragma once
#include "Json.hpp"

template<typename T, size_t N>
class JArray : public JsonArray, public std::array<T, N> {
public:
    void load(jstring jsnArray) override;
    jstring stringify() const override;
    operator std::array<T, N>& () { return *this; }
};

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