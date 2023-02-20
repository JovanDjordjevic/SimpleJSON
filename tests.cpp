#include "simpleJSON.hpp"

#include <cmath>
#include <cassert>

#include <iostream>

// helper function for comparing floats
template <typename T, typename V>
std::enable_if_t<std::is_floating_point_v<T> && std::is_floating_point_v<V>, bool>
equals(const T& f1, const V& f2) {
    double epsilon = 0.000001;
    return std::fabs(f2 - f1) < epsilon;
}

void testJSONString() {
    using namespace simpleJSON;

    const char* someString1 = "some string literal";
    std::string someString2("some std string");

    // constructors
    JSONString str1;                            assert(str1.getString() == "");
    JSONString str2(someString1);               assert(str2.getString() == someString1);
    JSONString str3(someString2);               assert(str3.getString() == someString2);
    JSONString str4 = someString1;              assert(str4.getString() == someString1);
    JSONString str5 = someString2;              assert(str5.getString() == someString2);
    JSONString str6 = str5;                     assert(str6.getString() == str5.getString());
    JSONString str7{JSONString(someString1)};   assert(str7.getString() == someString1);

    // assignment
    str1 = someString1;                         assert(str1.getString() == someString1);
    str1 = "someString1";                       assert(str1.getString() == "someString1");
    str1 = someString2;                         assert(str1.getString() == someString2);
    str1 = std::string("stdString");            assert(str1.getString() == std::string("stdString"));
    str1 = str5;                                assert(str1.getString() == str5.getString());

    // comparison
    assert(str1 == "some std string");
    assert(str1 == std::string("some std string"));
    assert(str1 != JSONString("something else"));
    assert(str1 != str4);
    assert(str1 == str5);
    assert(JSONString("aa") < JSONString("ab"));
    assert(JSONString("ab") <= JSONString("ab") && JSONString("ab") >= JSONString("ab"));
    assert(JSONString("ab") >= JSONString("aa"));
}

void testJSONNumber() {
    using namespace simpleJSON;

    JSONFloating f1 = 0.22e13;
    JSONIntegral ud1 = 123;
    JSONIntegral sd1 = -123;

    // constructors
    JSONNumber num1;                            assert(num1.getIntegral() == 0);
    JSONNumber num2(0.123);                     assert(equals(num2.getFloating(), 0.123));
    JSONNumber num3(f1);                        assert(equals(num3.getFloating(), f1));
    JSONNumber num4(1234151245);                assert(num4.getIntegral() == 1234151245);
    JSONNumber num5(ud1);                       assert(num5.getIntegral() == ud1);
    JSONNumber num6(-3514);                     assert(num6.getIntegral() == -3514);
    JSONNumber num7(sd1);                       assert(num7.getIntegral() == sd1);
    JSONNumber num8(num1);                      assert(num8.getIntegral() == 0);
    JSONNumber num9{JSONNumber(-0.14e23)};      assert(equals(num9.getFloating(), -0.14e23));

    // assignment
    num1 = 5;                                   assert(num1.getIntegral() == 5);
    num1 = -5;                                  assert(num1.getIntegral() == -5);
    num1 = f1;                                  assert(equals(num1.getFloating(), f1));
    num1 = num5;                                assert(num1.getIntegral() == ud1);
    num1 = JSONNumber{345};                     assert(num1.getIntegral() == 345);

    // comparison
    assert(num1 == 345);
    assert(num1 != num2);
    assert(JSONNumber(0.0001) == JSONNumber(0.0001));
    assert(num1 != num2);
    assert(num1 != -5);
    assert(num1 < 346);
    assert(num1 <= 346);
    assert(num1 > JSONNumber{2});
    assert(JSONNumber(99999) >= num1);
    assert(JSONNumber(-0.e2) < JSONNumber(12));
    assert(JSONNumber(123.324) >= JSONNumber(-33));
}

void testJSONBool() {
    using namespace simpleJSON;
    
    // constructors
    JSONBool b1;                                assert(b1.getBoolean() == false);
    JSONBool b2(true);                          assert(b2.getBoolean() == true);
    JSONBool b3 = false;                        assert(b3.getBoolean() == false);
    JSONBool b4{JSONBool(true)};                assert(b4.getBoolean() == true);

    // assignment
    b1 = true;                                  assert(b1.getBoolean() == true);
    b1 = JSONBool(false);                       assert(b1.getBoolean() == false);

    // comparison
    assert(b1 == false);
    assert(b1 == JSONBool{});
    assert(b1 != true);
    assert(b1 != JSONBool(true));
}

void testJSONNull() {
    using namespace simpleJSON;
    
    // constructors
    JSONNull n1;                                assert(n1.toString() == "null");
    JSONNull n2 = nullptr;                      assert(n2.toString() == "null");
    JSONNull n3 = NULL;                         assert(n3.toString() == "null");

    // comparison
    assert(JSONNull{} == JSONNull{});
    assert(n1 == nullptr);
    assert(n1 == NULL);
    assert(!(n1 != n2));
}

void testJSONArray() {
    using namespace simpleJSON;

    const char* someString1 = "some string literal";
    std::string someString2("some std string");
    JSONString jsonStr1("json str");
    JSONFloating f1 = 0.22e13;
    JSONIntegral ud1 = 123;
    JSONIntegral sd1 = -123;

    // constructors
    JSONArray arr;                                              assert(arr.size() == 0);

    // appending and clearing
    arr.append(someString1);                                    assert(arr.size() == 1);
    arr.append("some other string literal");                    assert(arr.size() == 2);
    arr.append(someString2);                                    assert(arr.size() == 3);
    arr.append(std::string("some other std string"));           assert(arr.size() == 4);
    arr.append(jsonStr1);                                       assert(arr.size() == 5);
    arr.append(JSONString("some other json str"));              assert(arr.size() == 6);
    arr.clear();                                                assert(arr.size() == 0);  
    arr.clear();                                                assert(arr.size() == 0);        
    arr.append(0.123);                                          assert(arr.size() == 1);
    arr.append(f1);                                             assert(arr.size() == 2);
    arr.append(1);                                              assert(arr.size() == 3);
    arr.append(ud1);                                            assert(arr.size() == 4);
    arr.append(-1);                                             assert(arr.size() == 5);
    arr.append(sd1);                                            assert(arr.size() == 6);
    arr.append(JSONNumber(123));                                assert(arr.size() == 7);
    arr.append(true);                                           assert(arr.size() == 8);
    arr.append(JSONBool{false});                                assert(arr.size() == 9);
    arr.append(nullptr);                                        assert(arr.size() == 10); 
    arr.append(JSONNull{});                                     assert(arr.size() == 11);
    arr.append(arr);                                            assert(arr.size() == 12);
    arr.append(JSONArray{});                                    assert(arr.size() == 13);
    arr.append(JSONObject());                                   assert(arr.size() == 14);
    arr.append(JSONObject("str obj"));                          assert(arr.size() == 15);

    // assignment
    JSONArray arr2;                                             assert(arr2.size() == 0);
    arr2.append("arr2 element");                                assert(arr2.size() == 1);
    arr2.append(0);                                             assert(arr2.size() == 2);
    JSONArray arr3 = arr2;                                      assert(arr3.size() == arr2.size());
    arr = arr3;
    arr3.clear();                                               assert(arr.size() == 2 && arr3.size() == 0);

    // subscription
    arr2[0] = 123;                                              
    arr2[1] = false;
    arr2.append(-2);
    arr2[2] = JSONNull{};

    // comparison
    assert(arr2 == arr2);
    assert(arr2 != arr3);
        JSONArray tmp;
        tmp.append(123);
        tmp.append(false);
        tmp.append(JSONNull{});
    assert(arr2 == tmp);
    assert(arr2 != JSONArray{});
    assert(arr2[0] >= tmp[0]);
}

void testJSONObject() {
    using namespace simpleJSON;

    const char* someString1 = "some string literal";
    std::string someString2("some std string");
    JSONString jsonStr1("some json str");
    JSONFloating f1 = 0.22e13;
    JSONIntegral ud1 = 123;
    JSONIntegral sd1 = -123;
    JSONNumber jsonNum1(-2);
    JSONBool jsonBool1(true);
    JSONArray jsonArr1;
    jsonArr1.append(JSONObject());
    jsonArr1.append(someString1);

    // constructors
    JSONObject obj0;
    JSONObject obj1("some other string literal");
    JSONObject obj2(someString1);
    JSONObject obj3(std::string("some other std string"));
    JSONObject obj4(someString2);
    JSONObject obj5(JSONString("some other json str"));
    JSONObject obj6(jsonStr1);
    JSONObject obj7(f1);
    JSONObject obj8(-0.124E5);
    JSONObject obj9(345);
    JSONObject obj10(ud1);
    JSONObject obj11(-345);
    JSONObject obj12(sd1);
    JSONObject obj13(JSONNumber(0));
    JSONObject obj14(jsonNum1);
    JSONObject obj15(true);
    JSONObject obj16(JSONBool{});
    JSONObject obj17(jsonBool1);
    JSONObject obj18(nullptr);
    JSONObject obj19(JSONNull{});
    JSONObject aaaaaa(NULL);
    JSONObject obj20(JSONArray{});
    JSONObject obj21(jsonArr1);
    JSONObject obj22(JSONObject{});
        JSONObject tmp;
        tmp["key1"] = JSONString("value1");
    JSONObject obj23(tmp);

    // assignment
    obj1 = "some String";
    obj1 = 123;
    obj1 = false;
    obj1 = JSONNull{};
    obj1 = jsonArr1;
    obj1 = obj23;

    // subscription
    JSONObject obj;
    obj["key1"] = "value1";
    obj[std::string("key2")] = std::string("value2");
    obj[JSONString("key3")] = JSONString("value3");
    obj["key1"] = -0.23;
    obj["key1"] = JSONNull{};
    obj["key4"] = JSONObject{};
    obj["key4"]["subkey1"]["subkey2"] = JSONArray{};
    obj["key4"]["subkey1"]["subkey2"].append("array element");
    obj["key4"]["subkey1"]["subkey2"].append(22);
    obj["newKey"] = obj["key4"]["subkey1"]["subkey2"];
    obj[std::string("key4")][JSONString("subkey1")]["subkey2"] = false;
    obj["numberField"] = -0.244;

    // comparison
    assert(obj1 == obj23);
    assert(obj1 != obj);
    assert(JSONObject(JSONNumber(0)) == obj13);
    assert(obj22 != obj5);
    assert(obj[std::string("key4")][JSONString("subkey1")]["subkey2"] == false);
        JSONArray tmpArr;
        tmpArr.append("array element");
        tmpArr.append(22);
    assert(obj["newKey"] == tmpArr);
    assert(obj5 == "some other json str");
    assert(obj5 <= "some other json str X"); 
    assert(obj["numberField"] > obj11);
}

void testStreamIO() {
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

int main () {
    testJSONString();
    testJSONNumber();
    testJSONBool();
    testJSONNull();
    testJSONArray();
    testJSONObject();

    testStreamIO();
    
    return 0;
}