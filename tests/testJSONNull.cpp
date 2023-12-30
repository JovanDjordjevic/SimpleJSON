#include "simpleJSON/simpleJSON.hpp"

#include <cassert>

void testJSONNull();

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


int main () {
    testJSONNull();

    return 0;
}
