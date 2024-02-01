#include "simpleJSON/simpleJSON.hpp"

#include <cmath>
#include <cassert>

void testJSONObject();

// helper function for comparing floats
template <typename T, typename V>
std::enable_if_t<std::is_floating_point_v<T> && std::is_floating_point_v<V>, bool>
equals(const T& f1, const V& f2) {
    double epsilon = 0.000001;
    return std::fabs(f2 - f1) < epsilon;
}

void testJSONObject() {
    using namespace simpleJSON;

    const char* someString1 = "some string literal";
    std::string someString2("some std string");
    JSONString jsonStr1("some json str");
    JSONFloating f1 = 0.22e13;
    JSONInteger ud1 = 123;
    JSONInteger sd1 = -123;
    JSONNumber jsonNum1(-2);
    JSONBool jsonBool1(true);
    JSONArray jsonArr1;
    jsonArr1.append(JSONObject());
    jsonArr1.append(someString1);

    // constructors
    JSONObject obj0;                                                        assert(obj0.size() == 0);
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
    JSONObject obj22(JSONObject{});                                         assert(obj22.size() == 0);
        JSONObject tmp;
        tmp["key1"] = JSONString("value1");
    JSONObject obj23(tmp);                                                  assert(obj23.size() == 1);
        const char* key2 = "key2";
        std::string key4 = "key4";
        JSONString key6("key6");
        JSONArray arr;
            arr.append(1);
            arr.append("str");
    JSONObject o = JSONMap{
            {"key1", "value1"}, 
            {key2, JSONString("value2")}, 
            {std::string("key3"), 234},
            {key4, JSONNull()},
            {JSONString("key5"), false},
            {"key6", arr},
            {"key7", JSONArray{1, "str", JSONNull{}}},
            {"key8", obj23},
            {"key9", JSONMap{{"k1", 1}, {"k2", 2}} },
            {"key10", JSONMap{{"k3", 3}, {"k4", 4}} }
        };

    assert(o.size() == 10);
    assert(o["key9"].size() == 2);
    o.removeField("key1");                                                  
    assert(o.size() == 9);
    assert(o["nonExistantField"].size() == 0);         // NOTE: operator[] will create a default json object if the key does not exist

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
    o["key6"].pop();                                                        assert(o["key6"].size() == 1);
    o["key6"][0] = "str";                                                   assert(o["key6"].size() == 1 && o["key6"][0] == "str");  
    o["key6"].clear();                                                      assert(o["key6"].size() == 0);  
    o.clear();                                                              assert(o.size() == 0);

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

int main () {
    testJSONObject();

    return 0;
}
