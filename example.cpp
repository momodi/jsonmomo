#include "jsonmomo.h"
#include <iostream>
using namespace std;
int main() {
    string a = "{\"hello\":[\"json\", \"momo\", 123]}";
    auto a_json = jsonmomo::Value().parse(a);
    cout << a_json << endl;
    cout << a_json["hello"][1].asString() << endl;

    auto b_json = jsonmomo::Value(jsonmomo::Object{
            {"hello", "json"},
            {"arr", jsonmomo::Array{1,2,3}}
            });
    cout << b_json["arr"] << endl;

    return 0;
}

