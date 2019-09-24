#include "param_reader.hpp"
#include <iostream>

RobotCarParams::RobotCarParams(int argc, char *argv[]): Kp(0.030), 
                                                        Ki(0.0005), 
                                                        Kd(0.025), 
                                                        max_val(80), 
                                                        uri("ws://192.168.0.51:8088")
{
    //./bin/robotapp 0.030 0.0005 0.025 100 http://169.254.223.91 8088
    if (argc >= 5)
    {
        Kp = atof(argv[1]);
        Ki = atof(argv[2]);
        Kd = atof(argv[3]);
        max_val = atoi(argv[4]);
        std::cout << "PID params: Kp=" << Kp << " Ki=" << Ki << " Kd=" << Kd << " MaxVel=" << max_val << std::endl;
    }
    if (argc >= 7)
    {
        uri = "ws://" + std::string(argv[5]) + ":" + std::string(argv[6]);
        std::cout << "connecting to: " << uri << std::endl;
    }
}