#include <cassert>
#include <iostream>

#include "simpleJSON/simpleJSON.hpp"

void testStreamIO();

void testStreamIO() {
    // auto obj1 = simpleJSON::parseFromFile("testInputs/smallJson.json");
    auto obj1 = simpleJSON::parseFromFile("../../tests/testInputs/smallJson.json");
    std::string obj1Str = simpleJSON::dumpToString(obj1);
    std::cout << obj1Str << std::endl << std::endl;
    std::string obj1Pretty = simpleJSON::dumpToPrettyString(obj1);
    std::cout << obj1Pretty << std::endl << std::endl;
    auto obj1_1 = simpleJSON::parseFromString(obj1Str);
    auto obj1_2 = simpleJSON::parseFromString(obj1Pretty);
    assert(obj1_1 == obj1_2);

    simpleJSON::dumpToFile(obj1, "smallJsonDump.json");
    auto obj1_3 = simpleJSON::parseFromFile("smallJsonDump.json");
    assert(obj1 == obj1_3);
    simpleJSON::dumpToPrettyFile(obj1, "smallJsonPrettyDump.json");
    auto obj1_4 = simpleJSON::parseFromFile("smallJsonPrettyDump.json");
    assert(obj1 == obj1_4);
 
    // std::string filePath{"testInputs/mediumJson.json"};
    std::string filePath{"../../tests/testInputs/mediumJson.json"};
    auto obj2 = simpleJSON::parseFromFile(filePath);
    std::string obj2Str = simpleJSON::dumpToString(obj2);
    std::cout << obj2Str << std::endl << std::endl;
    std::string obj2Pretty = simpleJSON::dumpToPrettyString(obj2);
    // std::cout << obj2Pretty << std::endl << std::endl;
    auto obj2_1 = simpleJSON::parseFromString(obj2Str);
    auto obj2_2 = simpleJSON::parseFromString(obj2Pretty);
    assert(obj2_1 == obj2_2);

    // std::filesystem::path path{"testInputs/veryBigJson.json"};
    std::filesystem::path path{"../../tests/testInputs/veryBigJson.json"};
    auto obj3 = simpleJSON::parseFromFile(path);
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
