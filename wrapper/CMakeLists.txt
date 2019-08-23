add_library(gopigo-wrapper src/gopigo-wrapper.cpp)
target_include_directories(gopigo-wrapper PUBLIC src)
target_link_libraries(gopigo-wrapper ${CONAN_LIBS})
