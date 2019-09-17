add_executable(robotapp 
               src/main.cpp
               src/line_sensor.cpp
               src/color_sensor.cpp
               src/mini_pid.cpp
               src/ws_client.cpp
               src/param_reader.cpp
               src/robot_car.cpp)

target_compile_options(robotapp PUBLIC -w)               
target_link_libraries(robotapp ${CONAN_LIBS})