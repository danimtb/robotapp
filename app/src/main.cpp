#include <GoPiGo3.h> // for GoPiGo3
#include <stdio.h>   // for printf
#include <unistd.h>  // for usleep
#include <signal.h>  // for catching exit signals
#include <iostream>
#include "line_sensor.hpp"

GoPiGo3 GPG;

void exit_signal_handler(int signo);

int main()
{
  signal(SIGINT, exit_signal_handler);
  GPG.detect();
  GPG.offset_motor_encoder(MOTOR_LEFT, GPG.get_motor_encoder(MOTOR_LEFT));
  GPG.offset_motor_encoder(MOTOR_RIGHT, GPG.get_motor_encoder(MOTOR_RIGHT));
  LineSensor line_sensor("/dev/i2c-1");
  while (true)
  {
    int result = line_sensor.readSensor();
    for (int i=4;i>=0;i--)
      std::cout << line_sensor.getIntensity(i);
    std::cout << std::endl;
    //int32_t EncoderLeft = GPG.get_motor_encoder(MOTOR_LEFT);
    //int32_t EncoderRight = GPG.get_motor_encoder(MOTOR_RIGHT);
    //printf("Encoder Left: %6d  Right: %6d\n", EncoderLeft, EncoderRight);
    usleep(20000);
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