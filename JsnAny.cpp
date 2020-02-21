#include "include/Json.hpp"

using namespace std;

void JsnAny::JAnyArray::load(jstring jsnArray) {
    array.clear();
    auto list = jsonArrayParse(jsnArray);
    for (auto& elem : list) {
        void *val = nullptr;
        string type = "";
        if (elem.front() >=  '0' && elem.front() <= '9') {
            if (elem.find('.') != string::npos) { // float
                val = new float(stof(elem));
                type = typeid(float).name();
            } else {// int
                val = new int(stoi(elem));
                type = typeid(int).name();
            }
        } else if (elem.front() == '"') {
            if (elem.size() == 3) { // char 
                val = new char(elem[1]);
                type = typeid(char).name();
            } else { // string 
                val = new string(elem.substr(1, elem.size() - 2));
                type = typeid(string).name();
            }
        } else if (elem.front() == '[') { // array
            auto tmp = new JAnyArray;
            tmp->load(elem);
            val = tmp;
            type = "Array";
        } else if (elem.front() == '{') { // obj
            auto tmp = new JsnAny;
            tmp->load(elem);
            val = tmp;
            type = "Object";
        } else throw "invalide Json format";
        array[val] = type;
    }
}

jstring JsnAny::JAnyArray::stringify() const {
    string str("[");
    for (auto& [ptr, type] : array) {
        str += any_stringify(type, ptr) + ", ";
    }
    str.pop_back();
    str.back() = ']';
    return str;
}

jstring JsnAny::any_stringify(string type, void *val) {
    if (type == typeid(int).name()) return to_string(*(int *)val);
    if (type == typeid(char).name()) return string("\"") + *(char *)val + '"'; 
    if (type == typeid(bool).name()) return *(bool *)val ? "true" : "false";
    if (type == typeid(float).name()) return to_string(*(float *)val);
    if (type == typeid(string).name()) return '"' + *(string *)val + '"';
    if (type == "Object") return ((Json *)val)->stringify();
    if (type == "Array") return ((JsonArray *)val)->stringify();
    return "";
}

void *JsnAny::new_val(string type, void *val) {
    if (type == typeid(int).name()) return new int(*(int *)val);
    if (type == typeid(char).name()) return new char(*(char *)val);
    if (type == typeid(bool).name()) return new bool(*(bool *)val);
    if (type == typeid(float).name()) return new float(*(float *)val);
    if (type == typeid(string).name()) return new string(*(string *)val);
    if (type == "Object") return new JsnAny(*(Json *)val);
    if (type == "Array") return new JsnAny(*(JsonArray *)val);
    return nullptr;
}

JsnAny::JsnAny(const Json& jsn) {
    for (auto& [name, val] : jsn._json_map) {
        void *self_val = new_val(val.first, val.second);
        if (!self_val) throw "error TODO";
        _json_map[name] = {val.first, self_val};
    }
}

JsnAny::JsnAny(const JsonArray& jsn) {
    auto* val = new JAnyArray;
    val->load(jsn.stringify());
    unique_val = {val, "Array"};
}


JsnAny::JsnAny(const Json&& jsn) {
    for (auto& [name, val] : jsn._json_map) {
        void *self_val = new_val(val.first, val.second);
        if (!self_val) throw "error TODO";
        _json_map[name] = {val.first, self_val};
    }
}

JsnAny::JsnAny(const JsonArray&& jsn) {
    auto* val = new JAnyArray;
    val->load(jsn.stringify());
    unique_val = {val, "Array"};
}

void JsnAny::load(jstring jsn) {

}

JsnAny::~JsnAny() {
    for (auto& [name, var] : _json_map) {
        //delete var.first;
        free(var.second);
    }
    _json_map.clear();
    if (unique_val.ptr) free(unique_val.ptr);//delete unique_val;
}