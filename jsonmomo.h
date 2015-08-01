/**
 * Jsonmomo is a header-only JSON library for C++1y, provided by gaoyunxiang[#]weidian.com
 */
#pragma once
#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <sstream>
#include <cctype>
#include <cstring>
using namespace std;
namespace jsonmomo {
    enum Type {
        NUL, INT, DOUBLE, BOOL, STRING, ARRAY, OBJECT
    };
    class Value;
    typedef map<string, Value> Object;
    typedef vector<Value> Array;
    class Value {
        public:
            Value():_type(NUL) {};
            Value(int v):_type(INT), v_int64(v) {};
            Value(int64_t v):_type(INT), v_int64(v) {};
            Value(double v):_type(DOUBLE), v_double(v) {};
            Value(bool v):_type(BOOL), v_bool(v) {};
            Value(const string &v):_type(STRING), v_string(v) {};
            Value(string &&v):_type(STRING), v_string(move(v)) {};
            Value(const char *v):_type(STRING), v_string(v) {};
            Value(const vector<Value> &v);
            Value(vector<Value> &&v);
            Value(const map<string, Value> &v);
            Value(map<string, Value> &&v);

            int asInt() const; // throw an exception when the value is not integer, or bigger than 32 bit integer.
            int64_t asInt64() const; // throw an exception when the value is not integer.
            double asDouble() const;
            bool asBool() const;
            const string& asString() const;
            vector<Value>& asVector(); // return C++STL vector as an array, so you can use foo.asVector().push_back(123) to append a value, or any other function with vector
            const vector<Value>& asVector() const;
            map<string, Value>& asMap(); // return C++Map as an Object.
            const map<string, Value>& asMap() const;
            Type type() const; // return the type of the value which is the set(NUL, INT, DOUBLE, BOOL, STRING, ARRAY, OBJECT)

            Value & operator[](size_t i);// no boundary check
            const Value & operator[](size_t i) const;
            Value & operator[](const string &key); //insert a null value when the key is not in the map
            const Value & operator[](const string &key) const; // throw exception when key is not in the map

            Value& parse(const string &in);
            string write() const;
            /**
             * rewrite operator <<, so you can do this: cout << foo << endl;
             */
            friend ostream & operator << (ostream & os, const Value &v) {
                os << v.write();
                return os;
            }
        private:
            Type _type;
            int64_t v_int64;
            double v_double;
            bool v_bool;
            string v_string;
            shared_ptr<vector<Value>> v_arr;
            shared_ptr<map<string, Value>> v_obj;
            void check_type(const Type &t) const;
            void rec_write(stringstream &O) const;
    };
    Value::Value(const vector<Value> &v) {
        this->_type = ARRAY;
        this->v_arr = shared_ptr<vector<Value>>(new vector<Value>());
        *(this->v_arr) = v;
    }
    Value::Value(vector<Value> &&v) {
        this->_type = ARRAY;
        this->v_arr = shared_ptr<vector<Value>>(new vector<Value>());
        *(this->v_arr) = move(v);
    }
    Value::Value(const map<string, Value> &v) {
        this->_type = OBJECT;
        this->v_obj = shared_ptr<map<string, Value>>(new map<string, Value>());
        *(this->v_obj) = v;
    }
    Value::Value(map<string, Value> &&v) {
        this->_type = OBJECT;
        this->v_obj = shared_ptr<map<string, Value>>(new map<string, Value>());
        *(this->v_obj) = move(v);
    }
    void Value::check_type(const Type &t) const {
        if (t != this->_type) {
            throw runtime_error("type error");
        }
    }
    int Value::asInt() const {
        check_type(INT);
        if ((int)this->v_int64 != this->v_int64) {
            throw runtime_error("Integer is too big than int32");
        }
        return this->v_int64;
    }
    int64_t Value::asInt64() const {
        check_type(INT);
        return this->v_int64;
    }
    double Value::asDouble() const {
        check_type(DOUBLE);
        return this->v_double;
    }
    bool Value::asBool() const {
        check_type(BOOL);
        return this->v_bool;
    }
    const string& Value::asString() const {
        check_type(STRING);
        return this->v_string;
    }
    vector<Value>& Value::asVector() {
        check_type(ARRAY);
        return *(this->v_arr);
    }
    const vector<Value>& Value::asVector() const {
        check_type(ARRAY);
        return *(this->v_arr);
    }
    map<string, Value>& Value::asMap() {
        check_type(OBJECT);
        return *(this->v_obj);
    }
    const map<string, Value>& Value::asMap() const {
        check_type(OBJECT);
        return *(this->v_obj);
    }
    Type Value::type() const {
        return this->_type;
    }
    Value & Value::operator[](size_t i) {
        check_type(ARRAY);
        return (*(this->v_arr))[i];
    }
    const Value & Value::operator[](size_t i) const {
        check_type(ARRAY);
        return (*(this->v_arr))[i];
    }
    Value & Value::operator[](const string &key) {
        check_type(OBJECT);
        return (*(this->v_obj))[key];
    }
    const Value & Value::operator[](const string &key) const {
        check_type(OBJECT);
        auto v = this->v_obj->find(key);
        if (v == this->v_obj->end()) {
            throw runtime_error("key:" + key + " missed");
        }
        return v->second;
    }
    Value& Value::parse(const string &in) {
        vector<pair<char, Value>> sta;
        sta.reserve(1000);
        const char *p = in.c_str();
        for (size_t i = 0; i < in.size(); ++i) {
            if (isspace(p[i]) || p[i] == '-' || p[i] == ':' || p[i] == ',') {
                continue;
            }
            if (p[i] == '[') {
                sta.emplace_back('[', Value());
            } else if (p[i] == '{') {
                sta.emplace_back('{', Value());
            } else if (p[i] == ']') {
                Value now{vector<Value>()};
                auto &vec = now.asVector();
                while (not sta.empty() && sta.back().first != '[') {
                    vec.emplace_back(move(sta.back().second));
                    sta.pop_back();
                }
                reverse(vec.begin(), vec.end());
                if (sta.empty() || sta.back().first != '[') {
                    throw runtime_error("parse error, [] not match");
                }
                sta.pop_back();
                sta.emplace_back('a', move(now));
            } else if (p[i] == '}') {
                Value now{map<string, Value>()};
                while (sta.size() >= 2 && sta.back().first != '{') {
                    pair<string, Value> a;
                    a.second = move(sta.back().second);
                    sta.pop_back();
                    if (sta.back().second.type() != STRING) {
                        throw runtime_error("parse error: some of the keys are not string");
                    }
                    a.first = move(sta.back().second.v_string);
                    sta.pop_back();
                    now.asMap().insert(a);
                }
                if (sta.empty() || sta.back().first != '{') {
                    throw runtime_error("parse error, {} not match");
                }
                sta.pop_back();
                sta.emplace_back('o', move(now));
            } else if (p[i] == '"') {
                size_t j = i + 1;
                string S;
                S.reserve(100);
                while (j < in.size() && p[j] != '"') {
                    if (p[j] == '\\') {
                        S.push_back(p[j + 1]);
                        j += 2;
                    } else {
                        S.push_back(p[j]);
                        j += 1;
                    }
                }
                sta.emplace_back('v', Value(S));
                i = j;
            } else if (isdigit(p[i])) {
                bool isdouble = false;
                for (auto j = i; j < in.size() && isdigit(p[j]); j += 1) {
                    if (p[j + 1] == '.' || p[j + 1] == 'e' || p[j + 1] == 'E') {
                        isdouble = true;
                        break;
                    }
                }
                int minus_flag = 1;
                if (i > 0 && p[i - 1] == '-') {
                    minus_flag = -1;
                }
                if (isdouble) {
                    double v = 0;
                    sscanf(p + i, "%lf", &v);
                    v *= minus_flag;
                    sta.emplace_back('d', Value(v));
                } else {
                    int64_t v = 0;
                    sscanf(p + i, "%ld", &v);
                    v *= minus_flag;
                    sta.emplace_back('i', Value(v));
                }
                while (i < in.size() && p[i] != ',' && p[i] != ']'  && p[i] != '}') {
                    i += 1;
                    continue;
                }
                i -= 1;
            } else if (strncmp("null", p + i, 4) == 0) {
                sta.emplace_back('n', Value());
                i += 3;
            } else if (strncmp("true", p + i, 4) == 0) {
                sta.emplace_back('b', Value(true));
                i += 3;
            } else if (strncmp("false", p + i, 4) == 0) {
                sta.emplace_back('b', Value(false));
                i += 4;
            } else {
                throw runtime_error(string("parse error:unrecognized character") + (p + i));
            }
        }
        if (sta.size() == 1) {
            *this = move(sta.back().second);
        } else {
            throw runtime_error("parse error, type=final");
        }
        return *this;
    }
    void Value::rec_write(stringstream &O) const {
        if (this->_type == NUL) {
            O << "null";
        } else if (this->_type == INT) {
            O << this->v_int64;
        } else if (this->_type == DOUBLE) {
            O << this->v_double;
        } else if (this->_type == BOOL) {
            if (this->v_bool == true) {
                O << "true";
            } else {
                O << "false";
            }
        } else if (this->_type == STRING) {
            O << '"';
            for (const auto &each: this->v_string) {
                if (each == '\\' || each == '"') {
                    O << '\\';
                }
                O << each;
            }
            O << '"';
        } else if (_type == ARRAY) {
            O << '[';
            for (auto it = this->v_arr->begin(); it != this->v_arr->end(); ++it) {
                if (it != this->v_arr->begin()) {
                    O << ',';
                }
                it->rec_write(O);
            }
            O << ']';
        } else if (_type == OBJECT) {
            O << '{';
            for (auto it = this->v_obj->begin(); it != this->v_obj->end(); ++it) {
                if (it != this->v_obj->begin()) {
                    O << ',';
                }
                O << '"' << it->first << '"' << ':';
                it->second.rec_write(O);
            }
            O << '}';
        }
    }
    string Value::write() const {
        stringstream O;
        rec_write(O);
        return O.str();
    }
}
