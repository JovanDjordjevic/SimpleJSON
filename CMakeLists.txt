cmake_minimum_required(VERSION 3.16)

project(SimpleJSON VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

option(ENABLE_TESTING "Build the test executables" ON)
if (ENABLE_TESTING)
    message(STATUS "Building tests")
    enable_testing()
endif()

# compiler detection
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "8")
        message(FATAL_ERROR "\ng++ v8 or later is required")
    endif()

    set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -pedantic -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
## clang++
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "6.0")
        message(FATAL_ERROR "\nclang++ v6.0 or later is required")
    endif()

    set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -pedantic -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
## microsoft visual c++
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    if(NOT MSVC_VERSION GREATER_EQUAL 1915)
        message(FATAL_ERROR "\nMSVC++ v19.15 or later is required")
    endif()

    set(CMAKE_CXX_FLAGS_DEBUG_INIT "/Wall")
    set(CMAKE_CXX_FLAGS_RELEASE_INIT "/Ox")
else()
  message(FATAL_ERROR "\n\
    Currently supported compilers:\n\
    - g++ v8 or above\n\
    - clang++ v6.0 or above\n\
    - MSVC++ v19.15 or above\n\
")
endif()

add_subdirectory(include)
add_subdirectory(tests)