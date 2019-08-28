add_executable(robotapp 
               src/main.cpp
               src/line_sensor.cpp
               src/color_sensor.cpp
               src/server.cpp
               src/MiniPID.cpp)
target_link_libraries(robotapp ${CONAN_LIBS})