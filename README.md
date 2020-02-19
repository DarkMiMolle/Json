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
  JVarVal(str, string, "Hello world !");
public:
  Example() = default;
  Example(const Example& copy) {
    Json::operator=(copy);
  }
};

class Exp2: public Json {
private:
  JVar(array, JVector<int>);
  JVar(obj, Example);
};

int main() {
  Example exp1;
  exp1.load("{\"val\": 42}");
  // exp1 = {"val": 42, "str": "Hello world !"}
  Exp2 exp2;
  JArray<int, 3> tab{31, 08, 99};
  exp2.load("\array\": " + Json::serialize(tab) + "{\"obj\": " + exp1.stringify() + "}");
  // exp2 = {"array": [31, 08, 99], "obj": {"val": 42, "str": "Hello world !"}}
}
```

---

`class Json`: inherits from that class make your class Jsonable.

| methods                                    | parameters and return info                                   | description                                                  |
| :----------------------------------------- | :----------------------------------------------------------- | ------------------------------------------------------------ |
| `virtual jstring stringify() const`        | • return: a valide Json string                               | return the Json string of the current object.                |
| `virtual void load(jstring jsn)`           | • `jstring jsn`: a string wich represent a Json.             | load jsn in the current obj or throw an error.               |
| `static T* create<T>(jstring jsn)`         | • `T` the type to create (must be Jsonable)<br />• `jstring jsn`: a string wich represent a Json.<br />• return: a new pointer T | Try to create a new T object where we load jsn.<br />On failure, return `nullptr`. |
| `static jstring serialize<T>(const T val)` | • `T` the type to serialize (must be Jsonable)<br />• return: a valide Json string<br />• `T val`: the val to serialize | return the jstring made by val                               |

Jsonable type are the following:

- `bool`
- `char`
- `int`
- `float`
- `string`
- `Json` (`JsonAny` and evry inherit class from `Json`) TODO: JsonAny
- `JsonArray` (`JVector`, `JArray`, and evry inherit class from `JsonArray`)

