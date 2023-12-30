#include "simpleJSON/simpleJSON.hpp"

#include <cmath>
#include <cassert>

void testJSONNumber();

// helper function for comparing floats
template <typename T, typename V>
std::enable_if_t<std::is_floating_point_v<T> && std::is_floating_point_v<V>, bool>
equals(const T& f1, const V& f2) {
    double epsilon = 0.000001;
    return std::fabs(f2 - f1) < epsilon;
}

void testJSONNumber() {
    using namespace simpleJSON;

    JSONFloating f1 = 0.22e13;
    JSONInteger ud1 = 123;
    JSONInteger sd1 = -123;

    // constructors
    JSONNumber num1;                            assert(num1.getInteger() == 0);
    JSONNumber num2(0.123);                     assert(equals(num2.getFloating(), 0.123));
    JSONNumber num3(f1);                        assert(equals(num3.getFloating(), f1));
    JSONNumber num4(1234151245);                assert(num4.getInteger() == 1234151245);
    JSONNumber num5(ud1);                       assert(num5.getInteger() == ud1);
    JSONNumber num6(-3514);                     assert(num6.getInteger() == -3514);
    JSONNumber num7(sd1);                       assert(num7.getInteger() == sd1);
    JSONNumber num8(num1);                      assert(num8.getInteger() == 0);
    JSONNumber num9{JSONNumber(-0.14e23)};      assert(equals(num9.getFloating(), -0.14e23));

    // assignment
    num1 = 5;                                   assert(num1.getInteger() == 5);
    num1 = -5;                                  assert(num1.getInteger() == -5);
    num1 = f1;                                  assert(equals(num1.getFloating(), f1));
    num1 = num5;                                assert(num1.getInteger() == ud1);
    num1 = JSONNumber{345};                     assert(num1.getInteger() == 345);

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

int main () {
    testJSONNumber();

    return 0;
}
