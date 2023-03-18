set(LIBRARY_HEADERS "simpleJSON.hpp")
set(LIBRARY_INCLUDES  "./")

add_library(simpleJSON INTERFACE)

target_include_directories(simpleJSON INTERFACE ${LIBRARY_INCLUDES})