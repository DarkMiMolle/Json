#pragma once
#include "Json.hpp"

template<typename T>
class JVector : public JsonArray, public std::vector<T> {
    //using JVector = std::vector<T>;
public:
    void load(jstring jsnArray) override;
    jstring stringify() const override;
    operator std::vector<T>& () { return *this; }
};

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