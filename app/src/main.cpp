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
  //ColorSensor color_sensor("/dev/i2c-1");

  GPG.detect();
  GPG.offset_motor_encoder(MOTOR_LEFT, GPG.get_motor_encoder(MOTOR_LEFT));
  GPG.offset_motor_encoder(MOTOR_RIGHT, GPG.get_motor_encoder(MOTOR_RIGHT));

  //std::ofstream fout;
  //fout.open("sensors.dat", std::ios::app); // open file for appending
  //assert(!fout.fail());

  MiniPID pid = MiniPID(65, 1, 0);
	pid.setOutputLimits(-75,75);
	//pid.setOutputRampRate(10);
  double setpoint=0;
  double sensor=0;

  while (true)
  {
    std::string buffer = server.getMessage();
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
    else
    {
      int result = line_sensor.readSensor();
      sensor = 0;
      for (int i=0;i<5;i++) { 
          sensor += ((i-2)*(1024-line_sensor.getVal(i)))/1024.0;        
      }
      double out=pid.getOutput(sensor,setpoint);
      total_power = static_cast<int>(100.0 * left_y);
      double control_command = (-out/100.0)/2.0+0.5;

      std::cout << "sensor:" << sensor << " command:" << control_command << std::endl;

      if (control_command < 0.5)
        factor_left = 2.0 * control_command;
      if (control_command > 0.5)
        factor_right = 2.0 * (1.0 - control_command);

      motor_left = factor_left * total_power;
      motor_right = factor_right * total_power;
      std::cout << "power: " << total_power << " left: " << motor_left << " right: " << motor_right << std::endl;
    }
    GPG.set_motor_power(MOTOR_LEFT, motor_left);
    GPG.set_motor_power(MOTOR_RIGHT, motor_right);
    usleep(10000);
  }
  //fout.close(); //close file
  //assert(!fout.fail());
}
/*
  int result = line_sensor.readSensor();
  GPG.set_motor_power(MOTOR_LEFT, motor_left);
  GPG.set_motor_power(MOTOR_RIGHT, motor_right);
  fout << line_sensor.getIntensity(4) << ";"
        << line_sensor.getIntensity(3) << ";"
        << line_sensor.getIntensity(2) << ";"
        << line_sensor.getIntensity(1) << ";"
        << line_sensor.getIntensity(0) << ";"
        << motor_left << ";"
        << motor_right << ";" << std::endl;
  // int result = line_sensor.readSensor();
  // for (int i = 4; i >= 0; i--)
  //  std::cout << line_sensor.getIntensity(i);
  //std::cout << std::endl;
  //color_sensor.readSensor();
  */

void exit_signal_handler(int signo)
{
  if (signo == SIGINT)
  {
    GPG.reset_all();
    exit(-2);
  }
}
