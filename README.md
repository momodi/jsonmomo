## JsonMoMo
Jsonmomo is a header-only JSON library for C++11、C++14.
The header file, "jsonmomo.h", has no more than 400 lines.
Json Objects act as Values, which can represent any JSON Value: null, int, double, bool, string, array, object. Value can be parsed from a string and serialized back to the string.
Array was implemented by std::vector, and Object was implemented by std::map.
It's easy to contruct a Json Value with C++11 syntax{}.
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
Jsonmomo was designed to throw Exception than to coredump when encounters abnormal data.

### Benchmark
I parsed a text file which has 300000 json-formatted strings to in-memory Jsonmomo::Values, and then I serialized the Values back to strings and restored them to another text file.

With jsonmomo, the process cost time:

    4.51s user 0.47s system 99% cpu 4.975 total
With [jsoncpp][1], the process cost time:

    11.12s user 0.55s system 99% cpu 11.667 total
    


  [1]: https://github.com/open-source-parsers/jsoncpp
