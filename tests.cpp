#include <exception>
#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>

#include "simpleJSON.hpp"

void test() {
    // std::string strJson = "{\"mykey\" : \"myvalue\"}"; // need escape the quotes
    using namespace simpleJSON;
    
    JSONArray arr;

    arr.emplace(JSONNumber(JSONSignedDecimal(-1)));
    arr.emplace(JSONNumber(JSONUnsignedDecimal(1)));
    arr.emplace(JSONNumber(JSONFloating(1)));
    arr.emplace(JSONString("aaaaaa"));
    arr.emplace(JSONString(std::string("bbbbbb")));
    arr.emplace(JSONBool(true));
    arr.emplace(JSONBool(false));
    arr.emplace(JSONNull{});
    arr.emplace(JSONArray{});

    JSONObject obj;
    obj[JSONString("key1")] = JSONNumber(JSONUnsignedDecimal(2));
    obj[JSONString("key1")] = JSONArray();

    JSONObject obj2;
    obj2[JSONString("key2")] = obj;
    obj2[JSONString("key3")] = arr;
    obj2["key4"] = JSONString("123");
    obj2["key4"] = std::string("123");
    obj2.removeField(JSONString("key3"));


    arr.emplace(obj);
    arr.emplace(obj2);

    std::cout << arr.toString() << std::endl;
}

int main () {

    test();

    return 0;
}