## JsonMoMo
Jsonmomo is a header-only JSON library for C++11.
It has only 300 hundreds of lines.
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

### Benchmark
I parsed 300000 lines to Json Values, and then I serialized them to restore to a text file.
With jsonmomo, the cost time:

    **4.51s user 0.47s system 99% cpu 4.975 total**
With jsoncpp, the cost time:

    **4.51s user 0.47s system 99% cpu 4.975 total**
    
