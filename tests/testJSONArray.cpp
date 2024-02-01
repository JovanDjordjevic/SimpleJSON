#include "simpleJSON/simpleJSON.hpp"

#include <cmath>
#include <cassert>

void testJSONArray();

// helper function for comparing floats
template <typename T, typename V>
std::enable_if_t<std::is_floating_point_v<T> && std::is_floating_point_v<V>, bool>
equals(const T& f1, const V& f2) {
    double epsilon = 0.000001;
    return std::fabs(f2 - f1) < epsilon;
}

void testJSONArray() {
    using namespace simpleJSON;

    const char* someString1 = "some string literal";
    std::string someString2("some std string");
    JSONString jsonStr1("json str");
    JSONFloating f1 = 0.22e13;
    JSONInteger ud1 = 123;
    JSONInteger sd1 = -123;

    // constructors
    JSONArray arr;                                              assert(arr.size() == 0);
    JSONArray initialzedArray = {1, "str", JSONNull{}};         assert(initialzedArray.size() == 3);

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
    initialzedArray.append(sd1);                                assert(initialzedArray.size() == 4);
    initialzedArray.clear();                                    assert(initialzedArray.size() == 0);
    initialzedArray.append(JSONNumber(123));                    assert(initialzedArray.size() == 1);
    initialzedArray.append(true);                               assert(initialzedArray.size() == 2);
    initialzedArray.append(JSONBool{false});                    assert(initialzedArray.size() == 3);
    initialzedArray.append(nullptr);                            assert(initialzedArray.size() == 4); 
    initialzedArray.append(JSONNull{});                         assert(initialzedArray.size() == 5);
    initialzedArray.append(arr);                                assert(initialzedArray.size() == 6);
    initialzedArray.append(JSONArray{});                        assert(initialzedArray.size() == 7);
    initialzedArray.append(JSONObject());                       assert(initialzedArray.size() == 8);
    initialzedArray.append(JSONObject("str obj"));              assert(initialzedArray.size() == 9);
    initialzedArray.append(JSONMap{{"key1", "value1"}});        assert(initialzedArray.size() == 10);
    initialzedArray.pop();                                      assert(initialzedArray.size() == 9);
    initialzedArray.pop();                                      assert(initialzedArray.size() == 8);
    

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

int main () {
    testJSONArray();

    return 0;
}
