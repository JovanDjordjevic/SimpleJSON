set(SJ_SIMPLE_JSON_HEADERS "simpleJSON.hpp")
set(SJ_SIMPLE_JSON_INCLUDES  "./")

add_library(simpleJSON INTERFACE)
target_include_directories(simpleJSON INTERFACE ${SJ_SIMPLE_JSON_INCLUDES})
