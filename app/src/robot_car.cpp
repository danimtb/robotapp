#include "robot_car.hpp"
#include <sstream>
#include <algorithm>
#include <math.h>
#include <GoPiGo3.h>

RobotCar::RobotCar() : _motor_left(0),
                       _motor_right(0),
                       _power_difference(0),
                       _current_color(""),
                       _last_color(""),
                       _setpoint(2500),
                       _sensor(0),
                       _driving_dir(-1),
                       _robot_wheel_radius(3.0 / 100.0),
                       _robot_wheel_base_length(12.0 / 100.0),
                       _wheel_encoder_ticks_per_revolution(720),
                       _prev_x(0),
                       _prev_y(0),
                       _prev_theta(0),
                       _x(0),
                       _y(0),
                       _theta(0),
                       _d_center(0),
                       _travelled_distance(0),
                       _total_travelled_distance(0)
{
}

RobotCar::~RobotCar()
{

}

void RobotCar::Init(double kp, double ki, double kd, double max_val)
{
  _max_val = std::min(max_val, 100.0);
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
  GPG.reset();
}

void RobotCar::DriveCrazy()
{
  for (int i=0;i<4;i++) {
    GPG->set_motor_power(MOTOR_LEFT, 50);
    GPG->set_motor_power(MOTOR_RIGHT, -50);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    GPG->set_motor_power(MOTOR_LEFT, 50);
    GPG->set_motor_power(MOTOR_RIGHT, 50);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    GPG->set_motor_power(MOTOR_LEFT, -50);
    GPG->set_motor_power(MOTOR_RIGHT, -50);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  }
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
  _current_color = color_sensor.getColor();
  //std::cout << _current_color << std::endl;
  std::ostringstream dataStream;

  dataStream << _x << ";"
             << _y << ";"
             << _theta << ";"
             << _current_color << ";"
             << _total_travelled_distance;

  _sensor_data = dataStream.str();
}

bool RobotCar::EnterGreen()
{
  return (_current_color == "green" && _last_color == "unknown");
}

bool RobotCar::ExitGreen()
{
  return (_current_color == "unknown" && _last_color == "green");
}

bool RobotCar::ExitPink()
{
  return (_current_color == "unknown" && _last_color == "pink");
}

bool RobotCar::EnterPink()
{
  return (_current_color == "pink" && _last_color == "unknown");
}

bool RobotCar::ExitOrange()
{
  return (_current_color == "unknown" && _last_color == "orange");
}

bool RobotCar::EnterOrange()
{
  return (_current_color == "orange" && _last_color == "unknown");
}

void RobotCar::TurnRight()
{
  _driving_dir = 0; // turn right
  _travelled_distance = 0;
}

void RobotCar::TurnLeft()
{
  _driving_dir = 2; // turn left
  _travelled_distance = 0;
}

void RobotCar::DriveStraight()
{
  _driving_dir = 1; // straight
  _travelled_distance = 0;
}

void RobotCar::StopForSeconds(int seconds)
{
  std::cout << "green" << std::endl;
  Stop();
  _total_travelled_distance = 0;
  std::this_thread::sleep_for(std::chrono::seconds(seconds));
  _travelled_distance = 0;
  _last_color = _current_color;
}

int RobotCar::Update()
{
  _travelled_distance = _travelled_distance + _d_center;
  _total_travelled_distance = _total_travelled_distance + _d_center;
  //std::cout << "path_sum: " << _travelled_distance << " driving dir: " << _driving_dir << std::endl;
  if (fabs(_travelled_distance) < 0.15 && _driving_dir >= 0)
  {
    std::cout << "path_sum: " << _travelled_distance << " driving dir: " << _driving_dir << std::endl;
    if (_driving_dir == 0)
    {
      _motor_left = _max_val;
      _motor_right = _max_val / 1.3;
    }
    else if (_driving_dir == 1)
    {
      _motor_left = _max_val / 1.1;
      _motor_right = _max_val;
    }
    else if (_driving_dir == 2)
    {
      _motor_left = _max_val / 1.5;
      _motor_right = _max_val;
    }
  }
  else if (_driving_dir >= 0)
  {
    _driving_dir = -1;
    _travelled_distance = 0;
  }
  _last_color = _current_color;
  // brake a little bit when reaching the end
  if (_total_travelled_distance > 3.9 && _total_travelled_distance < 4.15)
  {
    _motor_left = std::max(_motor_left * 2.0 * fabs(4.15 - _total_travelled_distance), _motor_left / 1.8);
    _motor_right = std::max(_motor_right * 2.0 * fabs(4.15 - _total_travelled_distance), _motor_right / 1.8);
  }
  GPG->set_motor_power(MOTOR_LEFT, _motor_left);
  GPG->set_motor_power(MOTOR_RIGHT, _motor_right);
  return 1;
}

std::string RobotCar::getSensorData()
{
  return _sensor_data;
}

// from https://github.com/nmccrea/sobot-rimulator
void RobotCar::UpdateOdometry()
{
  const double PI = 3.141592653589793;

  double R = _robot_wheel_radius;
  double N = _wheel_encoder_ticks_per_revolution;

  // read the wheel encoder values
  double ticks_left = GPG->get_motor_encoder(MOTOR_LEFT);
  double ticks_right = GPG->get_motor_encoder(MOTOR_RIGHT);

  // get the difference in ticks since the last iteration
  double d_ticks_left = ticks_left - _prev_ticks_left;
  if (d_ticks_left < 0)
    d_ticks_left = ticks_left;
  double d_ticks_right = ticks_right - _prev_ticks_right;
  if (d_ticks_right < 0)
    d_ticks_right = ticks_right;

  // estimate the wheel movements
  double d_left_wheel = 2.0 * PI * R * (d_ticks_left / N);
  double d_right_wheel = 2.0 * PI * R * (d_ticks_right / N);
  _d_center = 0.5 * (d_left_wheel + d_right_wheel);

  // calculate the new pose
  _x = _prev_x + (_d_center * cos(_prev_theta));
  _y = _prev_y + (_d_center * sin(_prev_theta));
  _theta = _prev_theta + ((d_right_wheel - d_left_wheel) / _robot_wheel_base_length);

  // update the pose estimate with the new values
  _prev_x = _x;
  _prev_y = _y;
  _prev_theta = _theta;

  // save the current tick count for the next iteration
  _prev_ticks_left = ticks_left;
  _prev_ticks_right = ticks_right;
}
