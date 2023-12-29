if(SJ_ENABLE_BUILD_EXAMPLES)
    set(SJ_TEST_SOURCES "example01.cpp")

    foreach(SJ_SOURCE IN LISTS SJ_TEST_SOURCES)
        get_filename_component(SJ_EXAMPLE_NAME ${SJ_SOURCE} NAME_WLE)
        message(STATUS "Creating example: ${SJ_EXAMPLE_NAME} from source: ${SJ_SOURCE}")
        
        add_executable(${SJ_EXAMPLE_NAME} ${SJ_SOURCE})
        target_link_libraries(${SJ_EXAMPLE_NAME} PUBLIC simpleJSON)
    endforeach()
endif()
