CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(ServoApp)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

if(WIRINGPI)
    add_definitions(-DWIRINGPI)
endif()

message("LIBS: ${CONAN_LIBS}")

ADD_EXECUTABLE(servoapp main.cpp)
target_link_libraries(servoapp ${CONAN_LIBS})
