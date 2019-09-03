#include <GoPiGo3.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>
#include <math.h>
#include <iterator>
#include <fstream>
#include <assert.h>
#include "line_sensor.hpp"
#include "color_sensor.hpp"
#include "server.hpp"
#include "MiniPID.h"

GoPiGo3 GPG;

void exit_signal_handler(int signo);

int main()
{
  signal(SIGINT, exit_signal_handler);
  Server server;
  server.init();
  LineSensor line_sensor("/dev/i2c-1");
  //ColorSensor color_sensor("/dev/i2c-1",TCS34725_INTEGRATIONTIME_50MS,TCS34725_GAIN_16X);
  //color_sensor.begin();
  GPG.detect();
  GPG.offset_motor_encoder(MOTOR_LEFT, GPG.get_motor_encoder(MOTOR_LEFT));
  GPG.offset_motor_encoder(MOTOR_RIGHT, GPG.get_motor_encoder(MOTOR_RIGHT));

  double Kp = 0.025;
  double Ki = 0.0001;
  double Kd = 0.00001;

  const int max_val = 100;

  MiniPID pid = MiniPID(Kp, Ki, Kd);
  pid.setOutputLimits(-max_val, max_val);

  double setpoint = 2500;
  double sensor = 0;
  while (true)
  {
    std::string buffer = "";
    buffer = server.getMessage();
    double left_y = 0;
    double right_x = 0;
    std::string state = buffer;
    std::vector<std::string> messages = split(state, '#');
    if (messages.size() > 0)
    {
      std::vector<std::string> tokens = split(messages[0], ';');
      if (tokens.size() > 3)
      {
        left_y = (128 - std::stoi(tokens[1])) / 128.00;
        right_x = std::stoi(tokens[2]) / 255.00;
      }
    }

    double threshold_val = 0.1;
    int total_power = 0;
    double factor_left = 1.0;
    double factor_right = 1.0;
    int motor_left = 0;
    int motor_right = 0;

    // MANUAL CONTROL
    /*
         if (fabs(right_x-0.5) > threshold_val && fabs(left_y) > threshold_val)
         {
         total_power = static_cast<int>(100.0 * left_y);
         if (right_x < 0.5)
         factor_left = 2.0 * right_x;
         if (right_x > 0.5)
         factor_right = 2.0 * (1.0 - right_x);
         
         motor_left = factor_left * total_power;
         motor_right = factor_right * total_power;
         std::cout << "power: " << total_power << " left: " << motor_left << " right: " << motor_right << std::endl;
         }
         // AUTOMATIC ALGORITHM
         else    */

    {
      int result = line_sensor.readSensor();
      int sensor = line_sensor.readLine();
      double power_difference = pid.getOutput(sensor, setpoint);


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

      std::cout << "sensor: " << sensor << " power difference: " << power_difference << " left: " << motor_left << " right: " << motor_right << std::endl;
    }
    GPG.set_motor_power(MOTOR_LEFT, motor_left);
    GPG.set_motor_power(MOTOR_RIGHT, motor_right);
    //uint16_t r, g, b, c, colorTemp, lux;
    //color_sensor.getRawData(&r, &g, &b, &c);
    //std::cout << r << " " << g << " " << b << " " << c << std::endl;
    usleep(3000);
  }

}

void exit_signal_handler(int signo)
{
  if (signo == SIGINT)
  {
    GPG.reset_all();
    exit(-2);
  }
}
