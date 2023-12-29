if(SJ_ENABLE_BUILD_TESTS)
    set(SJ_TEST_SOURCES
        "testJSONString.cpp"
        "testJSONNumber.cpp"
        "testJSONBool.cpp"
        "testJSONNull.cpp"
        "testJSONArray.cpp"
        "testJSONObject.cpp"
        "testStreamIO.cpp"
        "mustPassParsing.cpp"
        "mustFailParsing.cpp"
        "mayPassOrFailParsing.cpp"
        "mustTransformInput.cpp"
    )

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        if(SJ_ENABLE_BRUTAL_COMPILE_OPTIONS)
            include(brutal-compiler-options)

            set(SJ_WARNING_SUPPRESSORS)

            if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
                list(APPEND SJ_WARNING_SUPPRESSORS 
                    -Wno-unsafe-loop-optimizations
                )
            elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
                list(APPEND SJ_WARNING_SUPPRESSORS 
                    -Wno-padded
                    -Wno-date-time
                    -Wno-c++98-compat
                )
            elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
                list(APPEND SJ_WARNING_SUPPRESSORS 
                    /wd4820
                    /wd4514
                    /wd4711
                    /wd5045
                )
            endif()
        endif()

        if(SJ_ENABLE_SANITIZERS)
            include(sanitize)
        endif()

        if(SJ_ENABLE_CALLGRIND_TARGETS)
            include(grind)
        endif()

        if(SJ_ENABLE_STATIC_ANALYSIS)
            include(static-analysis)
        endif()
    endif()

    foreach(SJ_SOURCE IN LISTS SJ_TEST_SOURCES)
        get_filename_component(SJ_TEST_NAME ${SJ_SOURCE} NAME_WLE)
        message(STATUS "Creating test: ${SJ_TEST_NAME} from source: ${SJ_SOURCE}")

        add_executable(${SJ_TEST_NAME} ${SJ_SOURCE})
        target_link_libraries(${SJ_TEST_NAME} PUBLIC simpleJSON)

        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            if(SJ_ENABLE_BRUTAL_COMPILE_OPTIONS)
                scm_add_brutal_compiler_options(${SJ_TEST_NAME} PUBLIC ${SJ_WARNING_SUPPRESSORS})
            endif()

            if(SJ_ENABLE_SANITIZERS)
                # scm_add_address_sanitizer_with_options(${SJ_TEST_NAME} PUBLIC)
                # scm_add_undefined_behavior_sanitizer_with_options(${SJ_TEST_NAME} PUBLIC)
                # scm_add_thread_sanitizer_with_options(${SJ_TEST_NAME} PUBLIC)
                # scm_add_memory_sanitizer_with_options(${SJ_TEST_NAME} PUBLIC)
            endif()

            if(SJ_ENABLE_CALLGRIND_TARGETS)
                scm_create_grind_target(${SJ_TEST_NAME})
            endif()

            if(SJ_ENABLE_STATIC_ANALYSIS)
                # this target_include_directories is only needed for the ${SC-TEST_NAME}-static-analyze target
                # because at this time it cannot extract include directories from the linked simpleContainers target
                # remove this line once that is working!
                target_include_directories(${SJ_TEST_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)

                scm_add_static_analysis_target(${SJ_TEST_NAME})
            endif()
        endif()

        add_test(NAME ${SJ_TEST_NAME} COMMAND ${SJ_TEST_NAME})

        if(SJ_ENABLE_COVERAGE_REPORT)
            target_code_coverage(${SJ_TEST_NAME} AUTO ALL)
        endif()
    endforeach()
endif()
