CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(robotapp)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_subdirectory(app)
