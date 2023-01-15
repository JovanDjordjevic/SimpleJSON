#include "simpleJSON.hpp"

#include <iostream>

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

    auto obj3 = simpleJSON::parseFromFile("testInputs/smallJson.json");
    std::cout << obj3.toString() << std::endl << std::endl;

    std::string obj3Str = obj3.toString();
    auto obj3_1 = simpleJSON::parseFromString(obj3Str);
    std::cout << obj3_1.toString() << std::endl << std::endl;
 
    auto obj4 = simpleJSON::parseFromFile("testInputs/mediumJson.json");
    std::cout << obj4.toString() << std::endl << std::endl;

    auto obj5 = simpleJSON::parseFromFile("testInputs/veryBigJson.json");
    std::cout << obj5.toString() << std::endl << std::endl; 
}