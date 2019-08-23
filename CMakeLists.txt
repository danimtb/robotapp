CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(robotapp)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

message("LIBS: ${CONAN_LIBS}")

add_subdirectory(wrapper)
add_subdirectory(app)
