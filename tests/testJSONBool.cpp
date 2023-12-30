#include "simpleJSON/simpleJSON.hpp"

#include <cassert>

void testJSONBool();

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

int main () {
    testJSONBool();

    return 0;
}
