#include <GoPiGo3.h> // for GoPiGo3
#include <stdio.h>   // for printf
#include <unistd.h>  // for usleep
#include <signal.h>  // for catching exit signals
#include <iostream>
#include "line_sensor.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8080

GoPiGo3 GPG;

void exit_signal_handler(int signo);

int main()
{
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

  signal(SIGINT, exit_signal_handler);
  GPG.detect();
  GPG.offset_motor_encoder(MOTOR_LEFT, GPG.get_motor_encoder(MOTOR_LEFT));
  GPG.offset_motor_encoder(MOTOR_RIGHT, GPG.get_motor_encoder(MOTOR_RIGHT));
  LineSensor line_sensor("/dev/i2c-1");
  std::cout << "car inited" << std::endl;
  if ((new_socket = accept(server_fd, (struct sockaddr *)&addr,
                           (socklen_t *)&addrlen)) < 0)
  {
    perror("accept");
    exit(EXIT_FAILURE);
  }
  while (true)
  {
    valread = read(new_socket, buffer, 1024);
    printf("%s\n", buffer);
    char key = buffer[0];
    switch (key)
    {
    case 'q':
      GPG.set_motor_power(MOTOR_LEFT, 100);
      GPG.set_motor_power(MOTOR_RIGHT, 100);
      std::cout << "forward" << std::endl;
      break;
    case 'a':
      GPG.set_motor_power(MOTOR_LEFT, -100);
      GPG.set_motor_power(MOTOR_RIGHT, -100);
      std::cout << "backwards" << std::endl;
      break;
    case 'o':
      GPG.set_motor_power(MOTOR_LEFT, 50);
      GPG.set_motor_power(MOTOR_RIGHT, 100);
      std::cout << "left" << std::endl;
      break;
    case 'p':
      GPG.set_motor_power(MOTOR_LEFT, 100);
      GPG.set_motor_power(MOTOR_RIGHT, 50);
      std::cout << "right" << std::endl;
      break;
    case 's':
      GPG.set_motor_power(MOTOR_LEFT, 0);
      GPG.set_motor_power(MOTOR_RIGHT, 0);
      std::cout << "stop" << std::endl;
      break;
    }
    int result = line_sensor.readSensor();
      for (int i = 4; i >= 0; i--)
        std::cout << line_sensor.getIntensity(i);
    std::cout << std::endl;
    usleep(2000);
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

