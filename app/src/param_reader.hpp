#pragma once
#include <string>

class RobotCarParams
{
public:
	RobotCarParams() = default;
	~RobotCarParams () = default;
    RobotCarParams(int argc, char* argv[]);

    double Kp = 0.030;
    double Ki = 0.0005;
    double Kd = 0.01;
    int max_val = 50;
    std::string uri;
};