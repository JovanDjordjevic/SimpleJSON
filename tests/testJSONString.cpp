#include "simpleJSON/simpleJSON.hpp"

#include <cassert>

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

int main () {
    testJSONString();
    return 0;
}