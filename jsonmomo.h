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
using namespace std;
namespace jsonmomo {
    enum Type {
        NUL, INT, DOUBLE, BOOL, STRING, ARRAY, OBJECT
    };
    class Value {
        public:
            Value();
            Value(int v);
            Value(int64_t v);
            Value(double v);
            Value(bool v);
            Value(const string &v);
            Value(const char *v);
            Value(const vector<Value> &v);
            Value(const map<string, Value> &v);
            int asInt() const;
            int64_t asInt64() const;
            double asDouble() const;
            bool asBool() const;
            const string& asString() const;
            vector<Value>& asVector();
            const vector<Value>& asVector() const;
            map<string, Value>& asMap();
            const map<string, Value>& asMap() const;

            Type type() const;

            Value & operator[](size_t i);
            const Value & operator[](size_t i) const;
            Value & operator[](const std::string &key);
            const Value & operator[](const std::string &key) const;

            Value& parse(const string &in);
            string write() const;
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
            vector<Value> v_arr;
            map<string, Value> v_obj;
            void check_type(const Type &t) const;
            void rec_write(stringstream &O) const;
    };
    Value::Value() {
        this->_type = NUL;
    }
    Value::Value(int v) {
        this->_type = INT;
        this->v_int64 = v;
    }
    Value::Value(int64_t v) {
        this->_type = INT;
        this->v_int64 = v;
    }
    Value::Value(double v) {
        this->_type = DOUBLE;
        this->v_double = v;
    }
    Value::Value(bool v) {
        this->_type = BOOL;
        this->v_bool = v;
    }
    Value::Value(const string &v) {
        this->_type = STRING;
        this->v_string = v;
    }
    Value::Value(const char *v) {
        this->_type = STRING;
        this->v_string = v;
    }
    Value::Value(const vector<Value> &v) {
        this->_type = ARRAY;
        this->v_arr = v;
    }
    Value::Value(const map<string, Value> &v) {
        this->_type = OBJECT;
        this->v_obj = v;
    }
    void Value::check_type(const Type &t) const {
        if (t != this->_type) {
            throw std::runtime_error("type error");
        }
    }
    int Value::asInt() const {
        check_type(INT);
        if ((int)this->v_int64 != this->v_int64) {
            throw std::runtime_error("Integer is too big than int32");
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
    const string& Value::asString() const {
        check_type(STRING);
        return this->v_string;
    }
    vector<Value>& Value::asVector() {
        check_type(ARRAY);
        return this->v_arr;
    }
    const vector<Value>& Value::asVector() const {
        check_type(ARRAY);
        return this->v_arr;
    }
    map<string, Value>& Value::asMap() {
        check_type(OBJECT);
        return this->v_obj;
    }
    const map<string, Value>& Value::asMap() const {
        check_type(OBJECT);
        return this->v_obj;
    }
    Type Value::type() const {
        return this->_type;
    }
    Value & Value::operator[](size_t i) {
        check_type(ARRAY);
        return this->v_arr[i];
    }
    const Value & Value::operator[](size_t i) const {
        check_type(ARRAY);
        return this->v_arr[i];
    }
    Value & Value::operator[](const std::string &key) {
        check_type(OBJECT);
        return this->v_obj[key];
    }
    const Value & Value::operator[](const std::string &key) const {
        check_type(OBJECT);
        auto v = this->v_obj.find(key);
        if (v == this->v_obj.end()) {
            throw std::runtime_error("key:" + key + " missed");
        }
        return v->second;
    }
    Value& Value::parse(const string &in) {
        vector<pair<char, Value>> sta;
        const char *p = in.c_str();
        for (size_t i = 0; i < in.size(); ++i) {
            if (isspace(p[i])) {
                continue;
            }
            if (p[i] == '[') {
                sta.push_back({'[', Value()});
            } else if (p[i] == '{') {
                sta.push_back({'{', Value()});
            } else if (p[i] == ']') {
                Value now{vector<Value>()};
                auto &vec = now.asVector();
                while (not sta.empty() && sta.back().first != '[') {
                    vec.push_back(sta.back().second);
                    sta.pop_back();
                }
                reverse(vec.begin(), vec.end());
                if (sta.empty() || sta.back().first != '[') {
                    throw std::runtime_error("parse error, [] not match");
                }
                sta.pop_back();
                sta.push_back({'a', now});
            } else if (p[i] == '}') {
                Value now{map<string, Value>()};
                while (sta.size() >= 2 && sta.back().first != '{') {
                    pair<string, Value> a;
                    a.second = sta.back().second;
                    sta.pop_back();
                    a.first = sta.back().second.v_string;
                    sta.pop_back();
                    now.asMap().insert(a);
                }
                if (sta.empty() || sta.back().first != '{') {
                    throw std::runtime_error("parse error, {} not match");
                }
                sta.pop_back();
                sta.push_back({'o', now});
            } else if (p[i] == '"') {
                size_t j = i + 1;
                while (j < in.size() && p[j] != '"') {
                    j += 1;
                }
                sta.push_back({'v', Value(string(p + i + 1, p + j))});
                i = j;
            } else if (isdigit(p[i])) {
                bool isdouble = false;
                for (auto j = i; j < in.size() && isdigit(p[j]) && p[j + 1] == '.'; j += 1) {
                    isdouble = true;
                    break;
                }
                int minus_flag = 1;
                if (i > 0 && p[i - 1] == '-') {
                    minus_flag = -1;
                }
                if (isdouble) {
                    double v = 0;
                    sscanf(p + i, "%lf", &v);
                    v *= minus_flag;
                    sta.push_back({'d', Value(v)});
                } else {
                    int64_t v = 0;
                    sscanf(p + i, "%lld", &v);
                    v *= minus_flag;
                    sta.push_back({'i', Value(v)});
                }
                while (i < in.size() && p[i] != ',' && p[i] != ']'  && p[i] != '}') {
                    i += 1;
                    continue;
                }
                i -= 1;
            }
        }
        if (sta.size() == 1) {
            *this = sta.back().second;
        } else {
            throw std::runtime_error("parse error, type=final");
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
            O << this->v_bool;
        } else if (this->_type == STRING) {
            O << '"' << this->v_string << '"';
        } else if (_type == ARRAY) {
            O << '[';
            for (auto it = this->v_arr.begin(); it != this->v_arr.end(); ++it) {
                if (it != this->v_arr.begin()) {
                    O << ',';
                }
                it->rec_write(O);
            }
            O << ']';
        } else if (_type == OBJECT) {
            O << '{';
            for (auto it = this->v_obj.begin(); it != this->v_obj.end(); ++it) {
                if (it != this->v_obj.begin()) {
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
