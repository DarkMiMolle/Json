# Json++
Json for C++

libJsonpp (libJson++) try to make Json easy to use in C++

---

to build the lib:

```shell
    mkdir build
    cd build && cmake .. && make && cd -
```

then the lib is in build directory.

it is named: _libJson.a_

to test some test i have made run: `./build/test/JsonTest`

---

## How to use !

The goal with Json++ is to use Json as if it was part of C++.

To do so, there are some _Macro_ and _class_ that are defined.

### _class_ Json

Every `class` must inherit from __Json__ to be Jsonable. It defines the methods `load(string)` wiche will try to load the given string as a Json, and `stringify()` wiche will return the object in Json string.

Json also implements static methods: `creat<T>(string)` wiche try to create the type T through the given string, and `serialize<T>(const T)` wiche will return the Json string of the given T.

### _Macros_

After inherited from Json, the way to declare attribute change a little.

To declare attributes that will be in the Json, you need to use the Macro: JVar(NAME, TYPE)

This macro will declare an attribute named: m_NAME of type TYPE and will add it to the Json mapping.

It is possible to change the behaviour of the declaration. By default Json++ add "m_" before the attribute name. but it is possible to change that by doing:

```c++
#undef VARNAME
#define VARNAME(name) ???##name
```

where ??? is what you want.

It is also possible to se a default value to the attribute like in classique c++. To do so, just use the macro: `JVarVal(NAME, TYPE, VAL)`. In that case, the attribute NAME of type TYPE will be set with the value VAL.

Notice that those Macros allow you to automatise the Json loading/stringifyng, but you can choose not to use them and juste override the 2 methods load and stringify.

### _class_ Array

Array are quite special, and there are 2 predefined classes to use in Json++. `JVector<T>` wiche is basically a `std::vector<T>` and  `JArray<T, size_t N>` wiche is basically a `std::array<T, size_t N>`.

To use another container, it must inherits from `JsonArray`, and so, implements methods `load` and  `stringify`.

<u>exemple of use</u>

```c++
#include <Json.hpp>

using namespace std;

class Example: public Json {
private:
  JVar(val, int);
  JVarVal(str, string);
public:
  Example() = default;
  Example(const Example& copy) {
    Json::operator=(copy);
  }
};

class Exp2: public Json {
private:
  JVar(array, JVector);
  JVar(obj, Example);
};

int main() {
  Example exp1;
  exp1.load("{\"val\": 42, \"str\": \"hello world !\"}");
  // exp1 = {"val": 42, "str": "hello world !"}
  Exp2 exp2;
  exp2.load(string() + "{\"obj\": " + exp1.stringify() + "}");
  // exp2 = {"array": [], "obj": {"val": 42, "str": "hello world !"}}
}
```

