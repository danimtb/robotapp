#include "robot_car.hpp"
#include <sstream>
#include <GoPiGo3.h>

RobotCar::RobotCar():_motor_left(0),
                     _motor_right(0),
                     _encoder_left(0),
                     _encoder_right(0),
                     _power_difference(0),
                     _current_color(""),
                     _last_color(""),
                     _setpoint(2500),
                     _sensor(0),
                     _driving_dir(-1)                     
{

}

RobotCar::~RobotCar() {

}

void RobotCar::Init(double kp, double ki, double kd, double max_val)
{
    _max_val = max_val; 
    GPG = std::make_unique<GoPiGo3>();
    GPG->detect();
    GPG->offset_motor_encoder(MOTOR_LEFT, GPG->get_motor_encoder(MOTOR_LEFT));
    GPG->offset_motor_encoder(MOTOR_RIGHT, GPG->get_motor_encoder(MOTOR_RIGHT));
    pid.InitPID(kp, ki, kd);
    pid.setOutputLimits(-_max_val, _max_val);
}

void RobotCar::Stop()
{
    std::cout << "stop motors" << std::endl;
    GPG->set_motor_power(MOTOR_LEFT, 0);
    GPG->set_motor_power(MOTOR_RIGHT, 0);
}

void RobotCar::Reset()
{
    GPG->reset_all();
}

void RobotCar::DriveCrazy()
{
    GPG->set_motor_power(MOTOR_LEFT, 30);
    GPG->set_motor_power(MOTOR_RIGHT, -30);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    GPG->set_motor_power(MOTOR_LEFT, 30);
    GPG->set_motor_power(MOTOR_RIGHT, 30);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    GPG->set_motor_power(MOTOR_LEFT, -30);
    GPG->set_motor_power(MOTOR_RIGHT, -30);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

void RobotCar::ReadSensors()
{
    int result = line_sensor.readSensor();
    _sensor = line_sensor.readLine();
    //std::cout << _sensor << std::endl;
    _power_difference = pid.getOutput(_sensor, _setpoint);
    if (_power_difference > _max_val)
      _power_difference = _max_val;
    if (_power_difference < -_max_val)
      _power_difference = -_max_val;

    if (_power_difference < 0)
    {
      _motor_left = _max_val + _power_difference;
      _motor_right = _max_val;
    }
    else
    {
      _motor_left = _max_val;
      _motor_right = _max_val - _power_difference;
    }
    _encoder_left = GPG->get_motor_encoder(MOTOR_LEFT) % 360;
    _encoder_right = GPG->get_motor_encoder(MOTOR_RIGHT) % 360;
    _current_color = color_sensor.getColor();
    //std::cout << _current_color << std::endl;
    std::ostringstream dataStream;
    dataStream << _sensor << ";"
               << _power_difference << ";"
               << _motor_left << ";"
               << _motor_right << ";"
               << _encoder_left << ";"
               << _encoder_right << ";"
               << _current_color;
    _sensor_data = dataStream.str();
}

void RobotCar::DriveNormal() {
  if (_current_color != _last_color && _current_color == "green")
    {
      std::cout << "green" << std::endl;
      Stop();
      std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    }
    if (_current_color == "unknown" && _last_color == "green")
    {
      std::cout << "green" << std::endl;
      _driving_dir = 0; // turn right
      GPG->offset_motor_encoder(MOTOR_LEFT, GPG->get_motor_encoder(MOTOR_LEFT));
      GPG->offset_motor_encoder(MOTOR_RIGHT, GPG->get_motor_encoder(MOTOR_RIGHT));
    }
    if (_current_color == "unknown" && _last_color == "pink")
    {
      std::cout << "pink" << std::endl;
      _driving_dir = 2; // turn left
      GPG->offset_motor_encoder(MOTOR_LEFT, GPG->get_motor_encoder(MOTOR_LEFT));
      GPG->offset_motor_encoder(MOTOR_RIGHT, GPG->get_motor_encoder(MOTOR_RIGHT));
    }
    if (_current_color == "unknown" && _last_color == "orange")
    {
      std::cout << "orange" << std::endl;
      _driving_dir = 1; // straight
      GPG->offset_motor_encoder(MOTOR_LEFT, GPG->get_motor_encoder(MOTOR_LEFT));
      GPG->offset_motor_encoder(MOTOR_RIGHT, GPG->get_motor_encoder(MOTOR_RIGHT));
    }
    //std::cout << GPG->get_motor_encoder(MOTOR_LEFT) << " " << GPG->get_motor_encoder(MOTOR_RIGHT) << std::endl;
    int path_sum = GPG->get_motor_encoder(MOTOR_LEFT) + GPG->get_motor_encoder(MOTOR_RIGHT);
    if (path_sum < 500 && _driving_dir >= 0)
    {
      std::cout << "path_sum: " << path_sum << std::endl;
      if (_driving_dir == 0)
      {
        _motor_left = _max_val;
        _motor_right = _max_val / 1.5;
      }
      else if (_driving_dir == 1)
      {
        _motor_left = _max_val;
        _motor_right = _max_val;
      }
      else if (_driving_dir == 2)
      {
        _motor_left = _max_val / 1.5;
        _motor_right = _max_val;
      }
    }
    else
    {
      _driving_dir = -1;
      line_sensor.resetMask();
      //std::cout << "reset" << std::endl;
    }
    //std::cout << "motor: " << _motor_left << " " << _motor_right << std::endl;
    _last_color = _current_color;
    GPG->set_motor_power(MOTOR_LEFT, _motor_left);
    GPG->set_motor_power(MOTOR_RIGHT, _motor_right);
}

std::string RobotCar::getSensorData() {
    return _sensor_data;
}