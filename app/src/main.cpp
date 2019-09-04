#include "seasocks/PrintfLogger.h"
#include "seasocks/Server.h"
#include "seasocks/StringUtil.h"
#include "seasocks/WebSocket.h"
#include "seasocks/util/Json.h"
#include "line_sensor.hpp"
#include "color_sensor.hpp"
//#include "server.hpp"
#include "MiniPID.h"
#include <GoPiGo3.h>

#include <cstring>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>
#include <math.h>
#include <iterator>
#include <fstream>
#include <assert.h>
#include <sstream>


GoPiGo3 GPG;
using namespace seasocks;

void exit_signal_handler(int signo);

class MyHandler : public WebSocket::Handler
{
public:
  explicit MyHandler(Server *server)
      : _server(server), _currentValue(0)
  {
    setValue(1);
  }

  virtual void onConnect(WebSocket *connection) override
  {
    _connections.insert(connection);
    connection->send(_currentSetValue.c_str());
    std::cout << "Connected: " << connection->getRequestUri()
              << " : " << formatAddress(connection->getRemoteAddress())
              << "\nCredentials: " << *(connection->credentials()) << "\n";
  }

  virtual void onData(WebSocket *connection, const char *data) override
  {
    if (0 == strcmp("die", data))
    {
      _server->terminate();
      return;
    }
    if (0 == strcmp("close", data))
    {
      std::cout << "Closing..\n";
      connection->close();
      std::cout << "Closed.\n";
      return;
    }

/*
    const int value = std::stoi(data) + 1;
    if (value > _currentValue)
    {
      setValue(value);
      for (auto c : _connections)
      {
        c->send(_currentSetValue.c_str());
      }
    }

*/
  }
  void send(std::string data)
  {
    for (auto c : _connections)
    {
      c->send(data.c_str());
    }
  }
  virtual void onDisconnect(WebSocket *connection) override
  {
    _connections.erase(connection);
    std::cout << "Disconnected: " << connection->getRequestUri()
              << " : " << formatAddress(connection->getRemoteAddress()) << "\n";
  }

private:
  std::set<WebSocket *> _connections;
  Server *_server;
  int _currentValue;
  std::string _currentSetValue;

  void setValue(int value)
  {
    _currentValue = value;
    _currentSetValue = makeExecString("set", _currentValue);
  }
};

int main()
{
  //return 0;
  signal(SIGINT, exit_signal_handler);



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


 auto logger = std::make_shared<PrintfLogger>();

  Server server(logger);

  auto handler = std::make_shared<MyHandler>(&server);
  server.addWebSocketHandler("/ws", handler);
  server.setStaticPath("/ws_test_web");
  if (!server.startListening(9090)) {
      std::cerr << "couldn't start listening\n";
      return 1;
  }
  int myEpoll = epoll_create(10);
  epoll_event wakeSeasocks = {EPOLLIN | EPOLLOUT | EPOLLERR, {&server}};
  epoll_ctl(myEpoll, EPOLL_CTL_ADD, server.fd(), &wakeSeasocks);  

  while (true)
  {
    constexpr auto maxEvents = 1;
    epoll_event events[maxEvents];
    auto res = epoll_wait(myEpoll, events, maxEvents, 50);
    if (res < 0) {
        std::cerr << "epoll returned an error\n";
        return 1;
    }
    for (auto i = 0; i < res; ++i) {
        if (events[i].data.ptr == &server) {
            auto seasocksResult = server.poll(0);
            if (seasocksResult == Server::PollResult::Terminated)
                return 0;
            if (seasocksResult == Server::PollResult::Error)
                return 1;
        }
    }    
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

    //std::cout << "sensor: " << sensor << " power difference: " << power_difference << " left: " << motor_left << " right: " << motor_right << std::endl;

    GPG.set_motor_power(MOTOR_LEFT, motor_left);
    GPG.set_motor_power(MOTOR_RIGHT, motor_right); 

    std::ostringstream dataStream;
    dataStream << sensor << ";" 
               << power_difference << ";"
               << motor_left << ";" 
               << motor_right << ";" 
               << GPG.get_motor_encoder(MOTOR_LEFT) << ";" 
               << GPG.get_motor_encoder(MOTOR_RIGHT);
    handler->send(dataStream.str());
    std::cout << dataStream.str() << std::endl;

    //uint16_t r, g, b, c, colorTemp, lux;
    //color_sensor.getRawData(&r, &g, &b, &c);
    //std::cout << r << " " << g << " " << b << " " << c << std::endl;
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
