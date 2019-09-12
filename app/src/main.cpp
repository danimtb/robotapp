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
#include <chrono>

GoPiGo3 GPG;

void exit_signal_handler(int signo);

void stop_motors()
{
  std::cout << "stop motors" << std::endl;
  GPG.set_motor_power(MOTOR_LEFT, 0);
  GPG.set_motor_power(MOTOR_RIGHT, 0);
}
void bug() {
  GPG.set_motor_power(MOTOR_LEFT, 100);
  GPG.set_motor_power(MOTOR_RIGHT, -100);
}

int main(int argc, char* argv[])
{
  // ./bin/robotapp 0.025 0.00001 0.01 75 169.254.39.5 8088
  double Kp = 0.030;
  double Ki = 0.0005;
  double Kd = 0.01;
  int max_val = 50;
  std::string uri = "ws://169.254.253.189:8088";
  
  if (argc >= 5) {
    Kp = atof(argv[1]);
    Ki = atof(argv[2]);
    Kd = atof(argv[3]);
    max_val = atoi(argv[4]); 
    std::cout << "PID params: Kp=" << Kp << " Ki=" << Ki << " Kd=" << Kd << " MaxVel=" << max_val << std::endl;
  }

  if (argc >= 7) {
    uri = "ws://" + std::string(argv[5]) + ":" + std::string(argv[6]);
    std::cout << "connecting to: " << uri << std::endl;
  } 

  signal(SIGINT, exit_signal_handler);
  wsClient client;
  client.connect(uri);

  LineSensor line_sensor("/dev/i2c-1");
  ColorSensor color_sensor("/dev/i2c-1", TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_16X);
  color_sensor.begin();
  GPG.detect();
  GPG.offset_motor_encoder(MOTOR_LEFT, GPG.get_motor_encoder(MOTOR_LEFT));
  GPG.offset_motor_encoder(MOTOR_RIGHT, GPG.get_motor_encoder(MOTOR_RIGHT));

  MiniPID pid = MiniPID(Kp, Ki, Kd);
  pid.setOutputLimits(-max_val, max_val);

  double setpoint = 2500;
  double sensor = 0;
  std::string last_color = "";

  unsigned int driving_dir = -1;
  while (true)
  {
    bug();
    int result = line_sensor.readSensor();
    int sensor = line_sensor.readLine();
    std::cout << sensor << std::endl;
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

    int encoder_left = GPG.get_motor_encoder(MOTOR_LEFT)%360;
    int encoder_right = GPG.get_motor_encoder(MOTOR_RIGHT)%360;
    std::string current_color=color_sensor.getColor();

    std::ostringstream dataStream;
    dataStream << sensor << ";"
              << power_difference << ";"
              << motor_left << ";"
              << motor_right << ";"
              << encoder_left << ";"
              << encoder_right << ";"
              << current_color;
    //std::cout << dataStream.str() << std::endl;
    //std::cout << pid.getErrorSum() << std::endl;
    client.send_message(dataStream.str());

    if (current_color!=last_color && current_color=="red") {
      std::cout << "red" << std::endl;
      stop_motors();
      std::this_thread::sleep_for(std::chrono::milliseconds(13000));
    }
    if (current_color=="unknown" && last_color=="red") {
      std::cout << "red" << std::endl;
      driving_dir = 0; // turn right
      GPG.offset_motor_encoder(MOTOR_LEFT, GPG.get_motor_encoder(MOTOR_LEFT));
      GPG.offset_motor_encoder(MOTOR_RIGHT, GPG.get_motor_encoder(MOTOR_RIGHT));
    }
    if (current_color=="unknown" && last_color=="green") {
      std::cout << "green" << std::endl;
      driving_dir = 2; // turn left
      GPG.offset_motor_encoder(MOTOR_LEFT, GPG.get_motor_encoder(MOTOR_LEFT));
      GPG.offset_motor_encoder(MOTOR_RIGHT, GPG.get_motor_encoder(MOTOR_RIGHT));
    }
    if (current_color=="unknown" && last_color=="cyan") {
      std::cout << "cyan" << std::endl;
      driving_dir = 1; // straight
      GPG.offset_motor_encoder(MOTOR_LEFT, GPG.get_motor_encoder(MOTOR_LEFT));
      GPG.offset_motor_encoder(MOTOR_RIGHT, GPG.get_motor_encoder(MOTOR_RIGHT));
    }    
    //std::cout << GPG.get_motor_encoder(MOTOR_LEFT) << " " << GPG.get_motor_encoder(MOTOR_RIGHT) << std::endl;
    int path_sum = GPG.get_motor_encoder(MOTOR_LEFT) + GPG.get_motor_encoder(MOTOR_RIGHT);
    if (path_sum<700 && driving_dir>=0) {
      std::cout << "path_sum: " << path_sum << std::endl;
      if (driving_dir==0) {
        motor_left = max_val;
        motor_right = max_val/1.8;
      }      
      else if (driving_dir==1) {
        motor_left = max_val;
        motor_right = max_val;
      }      
      else if (driving_dir==2) {
        motor_left = max_val/1.8;
        motor_right = max_val;
      }      
    }
    else {
      driving_dir = -1;
      line_sensor.resetMask();
      std::cout << "reset" << std::endl;
    }
    std::cout << "motor: " << motor_left << " " << motor_right << std::endl;
    last_color = current_color;
    GPG.set_motor_power(MOTOR_LEFT, motor_left);
    GPG.set_motor_power(MOTOR_RIGHT, motor_right);

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
