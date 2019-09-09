#include "line_sensor.hpp"
#include "color_sensor.hpp"
#include "mini_pid.hpp"
#include <GoPiGo3.h>
#include "ws_client.hpp"


#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <math.h>
#include <iterator>
#include <fstream>
#include <assert.h>

GoPiGo3 GPG;

void exit_signal_handler(int signo);

void stop_motors()
{
  std::cout << "stop motors" << std::endl;
  GPG.set_motor_power(MOTOR_LEFT, 0);
  GPG.set_motor_power(MOTOR_RIGHT, 0);
}

int main()
{
  signal(SIGINT, exit_signal_handler);
  wsClient client;
  std::string uri = "ws://169.254.74.2:8088";
  client.connect(uri);

  LineSensor line_sensor("/dev/i2c-1");
  ColorSensor color_sensor("/dev/i2c-1", TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_16X);
  color_sensor.begin();
  GPG.detect();
  GPG.offset_motor_encoder(MOTOR_LEFT, GPG.get_motor_encoder(MOTOR_LEFT));
  GPG.offset_motor_encoder(MOTOR_RIGHT, GPG.get_motor_encoder(MOTOR_RIGHT));

  double Kp = 0.027;
  double Ki = 0.00011;
  double Kd = 0.000011;
  const int max_val = 100;

  MiniPID pid = MiniPID(Kp, Ki, Kd);
  pid.setOutputLimits(-max_val, max_val);

  double setpoint = 2500;
  double sensor = 0;

  while (true)
  {
    int result = line_sensor.readSensor();
    int sensor = line_sensor.readLine();

    double power_difference = pid.getOutput(sensor, setpoint);
    int motor_left = 0;
    int motor_right = 0;
    if (power_difference > max_val)
      power_difference = max_val;
    if (power_difference < -max_val)
      power_difference = -max_val;

    if (power_difference < 0)
    {
      motor_left = max_val + power_difference;
      motor_right = max_val;
    }
    else
    {
      motor_left = max_val;
      motor_right = max_val - power_difference;
    }

    bool bug = false;
    if (bug) {
      GPG.set_motor_power(MOTOR_LEFT, 50);
      GPG.set_motor_power(MOTOR_RIGHT, -50);
    }
    else {
      GPG.set_motor_power(MOTOR_LEFT, motor_left);
      GPG.set_motor_power(MOTOR_RIGHT, motor_right);
    }
    float r=0, g=0, b=0;
    color_sensor.getRGB(&r, &g, &b);
    int encoder_left = GPG.get_motor_encoder(MOTOR_LEFT)%360;
    int encoder_right = GPG.get_motor_encoder(MOTOR_RIGHT)%360;

    std::ostringstream dataStream;
    dataStream << sensor << ";"
              << power_difference << ";"
              << motor_left << ";"
              << motor_right << ";"
              << encoder_left << ";"
              << encoder_right << ";"
              << r << ";"
              << g << ";"
              << b;
    std::cout << dataStream.str() << std::endl;
    client.send_message(dataStream.str());
  }
  client.join_thread();
}



void exit_signal_handler(int signo)
{
  if (signo == SIGINT)
  {
    stop_motors();
    GPG.reset_all();
    exit(-2);
  }
}
