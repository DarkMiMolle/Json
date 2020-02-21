#include "include/Json.hpp"
#include "include/JsonErr.hpp"

#include <vector>
#include <regex>
#include <iostream>

using namespace std;

vector<jstring> JsonArray::jsonArrayParse(jstring jsn) {
  assert(jsn.front() == '[');
  vector<jstring> toklist;
  jsn = jsn.substr(1);
  for(; jsn.back() != ']'; jsn = jsn.substr(0, jsn.size() - 2));
  while (jsn != "") {
    for (; jsn.front() == ' '; jsn = jsn.substr(1));
    if (jsn.front() == '"') {
      auto endStr = jsn.find('"', 1);
      while (jsn[endStr - 1] == '\\') endStr = jsn.find('"', endStr + 1);
      toklist.push_back(jsn.substr(0, endStr));
      auto next = jsn.find(',', endStr + 1);
      jsn = jsn.substr(next);
    } else {
      auto next = jsn.find(',');
      if (next != string::npos) {
        size_t i = 1;
        for (; jsn[next - i] == ' '; ++i);
        toklist.push_back(jsn.substr(0, next - i + 1));
        jsn = jsn.substr(next + 1);
      } else {
        next = jsn.find(']');
        if (next == string::npos) throw "incomplet array missing ']'";
        size_t i = 1;
        for (; jsn[next - i] == ' '; ++i);
        toklist.push_back(jsn.substr(0, next - i + 1));
        jsn = "";
      }
    }
  }
  return toklist;
}

struct Token {
  struct tok {
    string name;
    string jsnVal;
  };

  vector<tok> list;

  pair<jstring, jstring> getValObj(jstring njsn) {
    assert(njsn.front() == '{');
    int obracket = 0;
    bool isStr = false;
    for (size_t i = 0; i < njsn.size(); ++i) {
      if (njsn[i] == '"' && njsn[i - 1] != '\\') isStr = !isStr;
      else if (njsn[i] == '{' && !isStr) ++obracket;
      else if (njsn[i] == '}' && !isStr) --obracket;
      if (obracket == 0) {
        return {njsn.substr(0, i + 1), njsn.substr(i + 1)};}
    }
    throw "incomplete json (sub)obj";
  }

  pair<jstring, jstring> getVal(jstring njsn) {
    size_t space = 0;
    while(njsn[space] == ' ')++space;
    njsn = njsn.substr(space);

    if (njsn.front() == '{') {
      return getValObj(njsn);
    } else if (njsn.front() == '[') {
      // TODO: i am an array
    } else {
      auto find = njsn.find(','), str = njsn.find('"');
      if (str != jstring::npos) {
        auto str_end = njsn.find('"', str);
        while (njsn[str_end - 1] == '\\') { // searching the end of the jstring
            str_end = njsn.find('"', str_end);
        }
        while (find > str && find < str_end && find != jstring::npos) {
          find = njsn.find(',', find); // finding the ',' which mark a new variable
        }
      }
      auto val = njsn.substr(0, find);
      while (val.front() == ' ') {
        val = val.substr(1);
      }
      while(val.back() == ' ') {
        val = val.substr(0, val.size() - 2);
      }
      auto next = find != jstring::npos ? njsn.substr(find + 1) : "";
      return {val, next};
    }
    return {"", ""};
  }

  void tokenize(jstring jsn) {
    while (jsn.size()) {
      auto findedColone = jsn.find(':'), varStart = jsn.find('"');
      auto var  = jsn.substr(varStart + 1, findedColone);
      auto varEnd = var.find('"');
      var = var.substr(0, varEnd);
      // auto i = 0;
      // for (; (var[i] < 'a' && var[i] > 'z') || (var[i] < 'A' && var[i] > 'Z'); ++i);
      // var = var.substr(i + 1);
      // i = var.size() - 1;
      // for (; var[i] != '"'; --i);
      // var = var.substr(0, i);
      auto [val, next] = getVal(jsn.substr(findedColone + 1));
      list.push_back({var, val});
      jsn = next;
    }
  }

  Token(jstring jsn) {
    if (jsn.front() != '{' || jsn.back() != '}') throw "error uncomplete json"; // TODO: throw error
    jsn = jsn.substr(1, jsn.size() - 2);
    tokenize(jsn);
  }
};

map<string, function<void(void *, string)>> Json::_json_assign = {
    { // char
      string(typeid(char).name()), 
      [](void *dst, string val) {
        char& var = *(char *)dst;
        regex r{"\".\""};
        if (!regex_match(val, r)) throw TypeError("char", val); // TODO
        var = val[1];
      }
    },
    { // int
      string(typeid(int).name()),
      [](void *dst, string val) {
        int& var = *(int *)dst;
        regex r{"[0-9]+"}; // \\d == digit
        if (!regex_match(val, r)) {
          cout << val << '\n';
          throw TypeError("int", val); //TODO
        }
        var = stoi(val);
      }
    },
    { // bool
      string(typeid(bool).name()),
      [](void *dst, string val) {
        bool& var = *(bool *)dst;
        regex r{"true|false"};
        if (!regex_match(val, r)) throw TypeError("bool", val);
        var = val == "true";
      }
    },
    { // string
      string(typeid(string).name()),
      [](void *dst, string val) {
        string& var = *(string *)dst;
        if (val.front() == '"' && val.back() == '"') {
          var = val.substr(1, val.size() - 2);
          return;
        }
        throw TypeError("string", val);
      }
    },
    { // float
      string(typeid(float).name()),
      [](void *dst, string val) {
        float& var = *(float *)dst;
        regex r{"[0-9]+\\.[0-9]+"}; // \\d == digit
        if (!regex_match(val, r)) throw TypeError("float", val); // TODO
        var = stof(val);
      }
    },
    { // Object
      "Object",
      [](void * dst, string val) {
        Json& obj = *(Json *)dst;
        obj.load(val);
      }
    },
    { // Array
      "Array",
      [](void *dst, string val) {
        JsonArray& var = *(JsonArray *)dst;
        var.load(val);
      }
    }
};

#define ope_func_body(Type)\
Type& var = *(Type *)vself;\
Type& val = *(Type *)vfrom;\
var = val;

template <typename T>
pair<string, function<void(void *, void *)>> _set_ope_eq() {
  return {
    string(typeid(T).name()),
    [](void *vself, void *vfrom) {
      ope_func_body(T)
    }
  };
}

map<string, function<void(void *, void *)>> Json::_json_operator_eq = {
  _set_ope_eq<int>(),
  _set_ope_eq<string>(),
  _set_ope_eq<float>(),
  _set_ope_eq<char>(),
  {// Object
    "Object",
    [](void *vself, void *vfrom) {
      ope_func_body(Json)
    }
  },
  {//Array
    "Array",
    [](void *vself, void *vfrom) {

    }
  }
};

void Json::load(jstring jsn) {
  auto t = Token(jsn);
  for (auto& token : t.list) {
    auto [var, val] = token;
    _json_assign[_json_map[var].first](_json_map[var].second, val);
  }
}

map<string, function<jstring(void *)>> Json::_json_stringify = {
  { // char
      string(typeid(char).name()), 
      [](void *val) {
        char& var = *(char *)val;
        return string("\"") + var + "\"";
      }
    },
    { // int
      string(typeid(int).name()),
      [](void *val) {
        int& var = *(int *)val;
        return to_string(var);
      }
    },
    { // bool
      string(typeid(bool).name()),
      [](void * val) {
        bool& var = *(bool *)val;
        return var ? "true" : "false";
      }
    },
    { // string
      string(typeid(string).name()),
      [](void *val) {
        string& var = *(string *)val;
        return "\"" + var + "\"";
      }
    },
    { // float
      string(typeid(float).name()),
      [](void *val) {
        float& var = *(float *)val;
        return to_string(var);
      }
    },
    { // Object
      "Object",
      [](void *val) {
        Json& obj = *(Json *)val;
        return obj.stringify();
      }
    },
    { // Array
      "Array",
      [](void *val) {
        JsonArray& var = *(JsonArray *)val;
        return var.stringify();
      }
    }
};

jstring Json::stringify(short flags) const {
  //TODO action on flags
  if (_json_map.empty()) {
    return "no object";
  }
  string jsn{"{"};
  for (auto& [name, pair] : _json_map) {
    jsn += "\"" + name + "\": " + _json_stringify.at(pair.first)(pair.second) + ", ";
  }
  jsn.pop_back();
  jsn.back() = '}';
  return jsn;
}

