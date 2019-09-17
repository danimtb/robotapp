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
                     _driving_dir(-1),
                     _robot_wheel_radius(3/100.0),
                     _robot_wheel_base_length(12/100.0),
                     _wheel_encoder_ticks_per_revolution(360),
                     _prev_x(0), 
                     _prev_y(0), 
                     _prev_theta(0),
                     _x(0),
                     _y(0),
                     _theta(0),
                     _d_center(0),
                     _travelled_distance(0)
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

    dataStream << _x << ";"
               << _y << ";"
               << _theta << ";"
               << _current_color;
               
    _sensor_data = dataStream.str();
}

void RobotCar::DriveNormal() {

  UpdateOdometry();

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
    }
    if (_current_color == "unknown" && _last_color == "pink")
    {
      std::cout << "pink" << std::endl;
      _driving_dir = 2; // turn left
    }
    if (_current_color == "unknown" && _last_color == "orange")
    {
      std::cout << "orange" << std::endl;
      _driving_dir = 1; // straight
    }
    _travelled_distance = _travelled_distance + _d_center;
    std::cout << "path_sum: " << _travelled_distance << std::endl;
    if (_travelled_distance < 0.15 && _driving_dir >= 0)
    {
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
      _travelled_distance = 0;
    }
    _last_color = _current_color;
    GPG->set_motor_power(MOTOR_LEFT, _motor_left);
    GPG->set_motor_power(MOTOR_RIGHT, _motor_right);
}

std::string RobotCar::getSensorData() {
    return _sensor_data;
}

// from https://github.com/nmccrea/sobot-rimulator
void RobotCar::UpdateOdometry() {
    const double PI = 3.141592653589793;

    double R = _robot_wheel_radius;
    int N = _wheel_encoder_ticks_per_revolution;

    // read the wheel encoder values
    int ticks_left = GPG->get_motor_encoder(MOTOR_LEFT);
    int ticks_right = GPG->get_motor_encoder(MOTOR_RIGHT);

    // get the difference in ticks since the last iteration
    double d_ticks_left = ticks_left - _prev_ticks_left;
    double d_ticks_right = ticks_right - _prev_ticks_right;
    
    // estimate the wheel movements
    double d_left_wheel = 2.0*PI*R*( d_ticks_left / N );
    double d_right_wheel = 2.0*PI*R*( d_ticks_right / N );
    _d_center = 0.5 * ( d_left_wheel + d_right_wheel );

    // calculate the new pose
    _x = _prev_x + ( _d_center * cos( _prev_theta ) );
    _y = _prev_y + ( _d_center * sin( _prev_theta ) );
    _theta = _prev_theta + ( ( d_right_wheel - d_left_wheel ) / _robot_wheel_base_length );

    // update the pose estimate with the new values
    _prev_x = _x;
    _prev_y = _y;
    _prev_theta = _theta;

    // save the current tick count for the next iteration
    _prev_ticks_left = ticks_left;
    _prev_ticks_right = ticks_right;
}
