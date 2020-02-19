#include "../include/Json.hpp"
#include <vector>
#include <functional>
#include <iostream>
#include <type_traits>
#include <exception>

using namespace std;

static int RES_TEST = 0;

function<void(int)> test(string name, function<bool(void)> f) {
    struct results {
        int succed;
        int failed;
    };
    static auto score = new results{0, 0}; 
    if (name == "end") {
        return [=](int id) {
            cout << "\nEND OF TESTS\n"
            << score->succed + score->failed << " test(s) done !\n"
            << score->succed << " test(s) SUCCED !\n"
            << score->failed << " test(s) FAILED :(\n";
            RES_TEST = score->failed;
            delete score;
        };
    }
    return [=](int id) {
        ++id;
        if (name != "") {
            cout << "=============================================\n\n\n";
            cout << "START TEST SECTION: " << name << '\n';
            cout << "-------------------------------------------\n";
        }
        cout << "start test n°" << id << '\n';
        auto res = f();
        if (res) {
           cout << "SUCCES\n";
           score->succed++;
        } else {
            cout << "FAILED\n";
            score->failed++;
        }
        cout << "end test n°" << id << '\n';
        cout << "-------------------------------------------\n";
    };
}

function<void(int)> test(function<bool(void)> f) {
    return test("", f);
}

function<void(int)> test(string section) {
    return test(section, []() {return true;});
}

class Test : public Json {
private:
    JVarVal(test, int, 42);
    JVar(test2, string);
public:
    Test() = default;
    Test(const Test& t) {
        JsonInit(t);
    }
    Test(const Test&& t) {
        Json::operator=(t);
    }
    int& test() { return m_test; }
    string& test2() { return m_test2; }
};

class TestObj : public Json {
private:
    JVar(val, int);
    JVar(test, Test);
public:
    int& val() { return m_val; }
    Test& test() { return m_test; }
};

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
    Test t;
    TestObj t2;



    vector<function<void(int)>> tests = {
        test("Base Tests", [&](){
            bool res = t.test() == 42;
            string json = "{\"test\": 18}";
            try {
                cout << "load: " << json << '\n';
                t.load(json);
            } catch (exception& e) {
                cerr << e.what() << '\n';
                return false;
            } catch (char const* str) {
                cerr << str << '\n';
                return false;
            }
            return res && t.test() == 18;
        }),
        test([&](){
            string json = "{\"test2\": \"this is a test\"}";
            try {
                cout << "load: " << json << '\n';
                t.load(json);
            } catch (exception& e) {
                cerr << e.what() << '\n';
                return false;
            } catch (char const* str) {
                cerr << str << '\n';
                return false;
            }
            
            return t.test2() == "this is a test";
        }),
        test([&](){
            string json = "{\"test\": 31, \"test2\": \"hello there\"}";
            try {
                cout << "load: " << json << '\n';
                t.load(json);
            } catch (exception& e) {
                cerr << e.what() << '\n';
                return false;
            } catch (char const* str) {
                cerr << str << '\n';
                return false;
            }

            return t.test() == 31 && t.test2() == "hello there";
        }),
        test("Obj Tests", [&]() {
            string json = "{\"val\": 42, \"test\": {\"test\": 31, \"test2\": \"hello there\"}}";
            try {
                cout << "load: " << json << '\n';
                t2.load(json);
            } catch (exception& e) {
                cerr << e.what() << '\n';
                return false;
            } catch (char const* str) {
                cerr << str << '\n';
                return false;
            }  
            return t2.val() == 42 && t2.test().test() == 31 && t2.test().test2() == "hello there";
        }),
        test("Array Tests", [&]() {
            string json = "[42, 23]";
            cout << "try to create a JVector with: '" << json << "'\n";
            try {
                auto* tab = Json::create<JVector<int>>(json);
                vector<int> expected = {42, 23};
                auto get = *tab;
                delete tab;
                return get == expected;
            } catch (const char *e) {
                return false;
            } catch (exception& e) {
                return false;
            }
        }),
        test([&]() {
            string json = "[23, 04, 99]";
            cout << "create JArray<int, 3> with '" << json << "'\n";
            try {
                auto* tab = Json::create<JArray<int, 3>>(json);
                array<int, 3> expected = {23, 04, 99};
                auto get = *tab;
                delete tab;
                return get == expected;
            } catch (const char *e) {
                return false;
            } catch (exception& e) {
                return false;
            }
        }),
        test("Stringify Tests", [&]() {
            t2.val() = 42;
            t2.test().test() = 31;
            t2.test().test2() = "18/02/99";
            string expected = "{\"test\": {\"test\": 31, \"test2\": \"18/02/99\"}, \"val\": 42}",
                get = t2.stringify();
            cout << "expect: " << expected << '\n';
            cout << "have: " << get << '\n';
            return get == expected;
        }),
        test("end")
    };

    for (int i = 0; i < tests.size(); ++i) {
        auto& test = tests[i];
        test(i);
    }
    return RES_TEST;
}
