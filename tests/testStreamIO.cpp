#include "simpleJSON/simpleJSON.hpp"

#include <cassert>

#include <iostream>

void testStreamIO() {
    auto obj1 = simpleJSON::parseFromFile("testInputs/smallJson.json");
    std::string obj1Str = simpleJSON::dumpToString(obj1);
    std::cout << obj1Str << std::endl << std::endl;
    std::string obj1Pretty = simpleJSON::dumpToPrettyString(obj1);
    std::cout << obj1Pretty << std::endl << std::endl;
    auto obj1_1 = simpleJSON::parseFromString(obj1Str);
    auto obj1_2 = simpleJSON::parseFromString(obj1Pretty);
    assert(obj1_1 == obj1_2);
 
    auto obj2 = simpleJSON::parseFromFile("testInputs/mediumJson.json");
    std::string obj2Str = simpleJSON::dumpToString(obj2);
    std::cout << obj2Str << std::endl << std::endl;
    std::string obj2Pretty = simpleJSON::dumpToPrettyString(obj2);
    // std::cout << obj2Pretty << std::endl << std::endl;
    auto obj2_1 = simpleJSON::parseFromString(obj2Str);
    auto obj2_2 = simpleJSON::parseFromString(obj2Pretty);
    assert(obj2_1 == obj2_2);

    auto obj3 = simpleJSON::parseFromFile("testInputs/veryBigJson.json");
    std::string obj3Str = simpleJSON::dumpToString(obj3);
    std::cout << obj3Str << std::endl << std::endl;
    std::string obj3Pretty = simpleJSON::dumpToPrettyString(obj3);
    // std::cout << obj3Pretty << std::endl << std::endl;
    auto obj3_1 = simpleJSON::parseFromString(obj3Str);
    auto obj3_2 = simpleJSON::parseFromString(obj3Pretty);
    assert(obj3_1 == obj3_2);
}

int main () {
    testStreamIO();

    return 0;
}