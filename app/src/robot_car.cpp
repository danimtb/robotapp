#include "robot_car.hpp"
#include <sstream>
#include <GoPiGo3.h>

RobotCar::RobotCar() {

}

RobotCar::~RobotCar() {

}

void RobotCar::Init(double kp, double ki, double kd, double max_val)
{
    GPG = std::make_unique<GoPiGo3>();
    _max_val = max_val; 
    _current_color = "";
    _last_color = "";
    _setpoint = 2500;
    _sensor = 0;
    _driving_dir = -1;

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
    std::cout << _sensor << std::endl;
    double power_difference = pid.getOutput(_sensor, _setpoint);
    if (power_difference > _max_val)
      power_difference = _max_val;
    if (power_difference < -_max_val)
      power_difference = -_max_val;

    if (power_difference < 0)
    {
      _motor_left = _max_val + power_difference;
      _motor_right = _max_val;
    }
    else
    {
      _motor_left = _max_val;
      _motor_right = _max_val - power_difference;
    }
    int encoder_left = GPG->get_motor_encoder(MOTOR_LEFT) % 360;
    int encoder_right = GPG->get_motor_encoder(MOTOR_RIGHT) % 360;
    _current_color = color_sensor.getColor();

    std::ostringstream dataStream;
    dataStream << _sensor << ";"
               << power_difference << ";"
               << _motor_left << ";"
               << _motor_right << ";"
               << encoder_left << ";"
               << encoder_right << ";"
               << _current_color;
    _sensor_data = dataStream.str();
}

void RobotCar::DriveNormal() {
  if (_current_color != _last_color && _current_color == "red")
    {
      std::cout << "red" << std::endl;
      Stop();
      std::this_thread::sleep_for(std::chrono::milliseconds(13000));
    }
    if (_current_color == "unknown" && _last_color == "red")
    {
      std::cout << "red" << std::endl;
      _driving_dir = 0; // turn right
      GPG->offset_motor_encoder(MOTOR_LEFT, GPG->get_motor_encoder(MOTOR_LEFT));
      GPG->offset_motor_encoder(MOTOR_RIGHT, GPG->get_motor_encoder(MOTOR_RIGHT));
    }
    if (_current_color == "unknown" && _last_color == "green")
    {
      std::cout << "green" << std::endl;
      _driving_dir = 2; // turn left
      GPG->offset_motor_encoder(MOTOR_LEFT, GPG->get_motor_encoder(MOTOR_LEFT));
      GPG->offset_motor_encoder(MOTOR_RIGHT, GPG->get_motor_encoder(MOTOR_RIGHT));
    }
    if (_current_color == "unknown" && _last_color == "cyan")
    {
      std::cout << "cyan" << std::endl;
      _driving_dir = 1; // straight
      GPG->offset_motor_encoder(MOTOR_LEFT, GPG->get_motor_encoder(MOTOR_LEFT));
      GPG->offset_motor_encoder(MOTOR_RIGHT, GPG->get_motor_encoder(MOTOR_RIGHT));
    }
    //std::cout << GPG->get_motor_encoder(MOTOR_LEFT) << " " << GPG->get_motor_encoder(MOTOR_RIGHT) << std::endl;
    int path_sum = GPG->get_motor_encoder(MOTOR_LEFT) + GPG->get_motor_encoder(MOTOR_RIGHT);
    if (path_sum < 700 && _driving_dir >= 0)
    {
      std::cout << "path_sum: " << path_sum << std::endl;
      if (_driving_dir == 0)
      {
        _motor_left = _max_val;
        _motor_right = _max_val / 1.8;
      }
      else if (_driving_dir == 1)
      {
        _motor_left = _max_val;
        _motor_right = _max_val;
      }
      else if (_driving_dir == 2)
      {
        _motor_left = _max_val / 1.8;
        _motor_right = _max_val;
      }
    }
    else
    {
      _driving_dir = -1;
      line_sensor.resetMask();
      std::cout << "reset" << std::endl;
    }
    std::cout << "motor: " << _motor_left << " " << _motor_right << std::endl;
    _last_color = _current_color;
    GPG->set_motor_power(MOTOR_LEFT, _motor_left);
    GPG->set_motor_power(MOTOR_RIGHT, _motor_right);
}

std::string RobotCar::getSensorData() {
    return _sensor_data;
}