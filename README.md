## JsonMoMo
Jsonmomo is a header-only JSON library for C++11、C++14.
The header file, "jsonmomo.h", it has only 300 lines.
Json Object acts like a value which can represents any JSON Value: null, int, double, bool, string, array, object. Value can be parsed from a string and serialized to a string.

It's very easy to contruct a Json Value with C++11 syntax{}.
For more detail, see jsonmomo.h.



### Example
    string a = "{\"hello\":[\"json\", \"momo\", 123]}";
    auto a_json = jsonmomo::Value().parse(a);
    cout << a_json << endl;
    cout << a_json["hello"][1].asString() << endl;

    auto b_json = jsonmomo::Value(jsonmomo::Object{
            {"hello", "json"},
            {"arr", jsonmomo::Array{1,2,3}}
            });
    cout << b_json["arr"] << endl;

The results:

    {"hello":["json","momo",123]}
    momo
    [1,2,3]
    
### Abnormal Data
Jsonmomo was designed to throw Exception than to coredump when it encounters abnormal data.

### Core API
    int asInt() const; // throw an exception when the value is not integer, or bigger than 32 bit integer.
    int64_t asInt64() const; // throw an exception when the value is not integer.
    double asDouble() const; // integer and double are both ok.
    bool asBool() const;
    const string& asString() const;
    vector<Value>& asVector(); // return C++STL vector as an array, so you can use foo.asVector().push_back(123) to append a value, or any other function with vector
    const vector<Value>& asVector() const;
    map<string, Value>& asMap(); // return C++Map as an Object.
    const map<string, Value>& asMap() const;
    Type type() const; // return the type of the value, which is one of the set(NUL, INT, DOUBLE, BOOL, STRING, ARRAY, OBJECT)

    Value & operator[](size_t i);// no boundary check
    const Value & operator[](size_t i) const;
    Value & operator[](const string &key); //insert a null value when the key is not in the map
    const Value & operator[](const string &key) const; // throw exception when key is not in the map

    Value& parse(const string &in);
    string write() const;
    
### Benchmark
I parsed a text file which has 300000 json-formatted strings to in-memory Jsonmomo::Values, and then I serialized the Values to strings and restored them to another text file.

With jsonmomo, the process cost time:

    4.51s user 0.47s system 99% cpu 4.975 total
With [jsoncpp][1], the process cost time:

    11.12s user 0.55s system 99% cpu 11.667 total
    


  [1]: https://github.com/open-source-parsers/jsoncpp
