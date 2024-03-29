#pragma once

#include "line_sensor.hpp"
#include "color_sensor.hpp"
#include "mini_pid.hpp"
#include <iostream>
#include <memory>
#include <set>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <math.h>
#include <iterator>
#include <fstream>
#include <assert.h>
#include <chrono>
#include <thread>
#include <memory>

class GoPiGo3;

class RobotCar
{
public:
    RobotCar();
    ~RobotCar();

    void Stop();
    void Reset();
    void DriveCrazy();
    void Init(double kp, double ki, double kd, double max_val);
    void ReadSensors();
    int DriveNormal();
    void UpdateOdometry();
    LineSensor line_sensor;
    ColorSensor color_sensor;
    MiniPID pid;
    std::string getSensorData();
    bool EnterGreen();
    bool ExitGreen();
    bool EnterPink();
    bool ExitPink();
    bool EnterOrange();
    bool ExitOrange();
    int Update();
    void StopForSeconds(int seconds);
    void TurnRight();
    void TurnLeft();
    void DriveStraight();

private:
    std::unique_ptr<GoPiGo3> GPG;
    double _setpoint;
    double _sensor;
    std::string _last_color;
    std::string _current_color;
    int _driving_dir;
    std::string _sensor_data;
    double _max_val;
    int _motor_left;
    int _motor_right;    
    double _power_difference;
    double _robot_wheel_radius;
    double _wheel_encoder_ticks_per_revolution;
    int _prev_ticks_left;
    int _prev_ticks_right;
    double _prev_x, _prev_y, _prev_theta;
    double _x, _y, _theta;
    double _d_center;
    double _travelled_distance;
    double _total_travelled_distance;
    double _robot_wheel_base_length;
};