#include <GoPiGo3.h> // for GoPiGo3
#include <stdio.h>   // for printf
#include <unistd.h>  // for usleep
#include <signal.h>  // for catching exit signals
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <math.h>
#include <sstream>
#include <iterator>
#include <vector>
#include <fstream>
#include <assert.h>
#include "line_sensor.hpp"
#include "color_sensor.hpp"

#define PORT 8080

std::vector<std::string> split(const std::string& s, char delimiter)
{
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(s);
   while (std::getline(tokenStream, token, delimiter))
   {
      tokens.push_back(token);
   }
   return tokens;
}

GoPiGo3 GPG;

void exit_signal_handler(int signo);

int main()
{
  GPG.detect();
  GPG.offset_motor_encoder(MOTOR_LEFT, GPG.get_motor_encoder(MOTOR_LEFT));
  GPG.offset_motor_encoder(MOTOR_RIGHT, GPG.get_motor_encoder(MOTOR_RIGHT));
  LineSensor line_sensor("/dev/i2c-1");
  //ColorSensor color_sensor("/dev/i2c-1");

  int server_fd, new_socket, valread;
  struct sockaddr_in addr;
  int opt = 1;
  int addrlen = sizeof(addr);
  char buffer[1024] = {0};

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
  {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                 &opt, sizeof(opt)))
  {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(PORT);

  if (bind(server_fd, (struct sockaddr *)&addr,
           sizeof(addr)) < 0)
  {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  if (listen(server_fd, 3) < 0)
  {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  std::cout << "waiting for client..." << std::endl;
  if ((new_socket = accept(server_fd, (struct sockaddr *)&addr,
                           (socklen_t *)&addrlen)) < 0)
  {
    perror("accept");
    exit(EXIT_FAILURE);
  }
  signal(SIGINT, exit_signal_handler);
  std::ofstream fout;
  fout.open("sensors.dat",std::ios::app);    // open file for appending
  assert (!fout.fail( ));     
  while (true)
  {
    valread = read(new_socket, buffer, 1024);
    double left_y = 0;
    double right_x = 0;
    std::string state = buffer;
    std::vector<std::string> messages = split(state, '#');
    if (messages.size()>0) {
      std::vector<std::string> tokens = split(messages[0], ';');
      if (tokens.size()>3) {
        left_y = (128 - std::stoi(tokens[1])) / 128.00;
        right_x = std::stoi(tokens[2]) / 255.00;
      }
    }

    double threshold_val = 0.05;
    int total_power = 0;
    double factor_left=1.0;
    double factor_right=1.0;    
    int motor_left = 0;
    int motor_right = 0;    
    if (fabs(left_y) > threshold_val)
    {
      total_power  = static_cast<int>(100.0*left_y);
      if (right_x<0.5)
        factor_left = 2.0*right_x;
      if (right_x>0.5)
        factor_right = 2.0*(1.0-right_x);

      motor_left = factor_left*total_power;
      motor_right = factor_right*total_power;
      //std::cout << "power: " << total_power << " left: " << motor_left << " right: " << motor_right << std::endl;
    }
    int result = line_sensor.readSensor();
    GPG.set_motor_power(MOTOR_LEFT, motor_left);
    GPG.set_motor_power(MOTOR_RIGHT, motor_right);
    fout << line_sensor.getIntensity(4) << ";" 
         << line_sensor.getIntensity(3) << ";" 
         << line_sensor.getIntensity(2) << ";" 
         << line_sensor.getIntensity(1) << ";" 
         << line_sensor.getIntensity(0) << ";" 
         << motor_left  << ";" 
         << motor_right << ";" << std::endl;
    // int result = line_sensor.readSensor();
    // for (int i = 4; i >= 0; i--)
    //  std::cout << line_sensor.getIntensity(i);
    //std::cout << std::endl;
    //color_sensor.readSensor();
    usleep(2000);
  }
  fout.close( );       //close file
  assert(!fout.fail( ));
}

void exit_signal_handler(int signo)
{
  if (signo == SIGINT)
  {
    GPG.reset_all();
    exit(-2);
  }
}
