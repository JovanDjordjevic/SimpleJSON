#include <chrono>
#include <exception>
#include <filesystem>
#include <iostream>

#include "simpleJSON/simpleJSON.hpp"

int main() {

    std::filesystem::path dataPath("./test_parsing");

    std::cout << "===============================" << std::endl;
    std::cout << "Testing cases that are implementation defined" << std::endl;
    std::cout << "===============================" << std::endl;

    for(const auto& dirEntry : std::filesystem::directory_iterator(dataPath)) {
        auto fileName = dirEntry.path().stem();
        if (fileName.string()[0] == 'i') {
            std::cout << "Testing file: " << fileName << std::endl;

            auto start = std::chrono::high_resolution_clock::now();

            try {
                auto obj1 = simpleJSON::parseFromFile(dirEntry.path().string().c_str());
                std::cout << obj1.toString() << std::endl;
                
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> diff = end - start;
                std::cout << "\tFinished in " << diff.count() << std::endl << std::endl;
            }
            catch (std::exception& e) {
                std::cout << "\tException: " << e.what() << std::endl << std::endl;
                auto end = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double> diff = end - start;
                std::cout << "\tFinished in " << diff.count() << std::endl << std::endl;
            }
        }
    }   

    return 0;
}