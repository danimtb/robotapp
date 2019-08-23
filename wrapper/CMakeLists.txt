project(lib VERSION 0.1 LANGUAGES CXX)

add_library(gopigo-wrapper src/gopigo-wrapper.cpp)
target_include_directories(gopigo-wrapper PUBLIC include)
target_link_libraries(gopigo-wrapper ${CONAN_LIBS})