
#include "ws_client.hpp"
#include "robot_car.hpp"
#include "param_reader.hpp"

RobotCar robot_car;

void exit_signal_handler(int signo);

int main(int argc, char *argv[])
{
      
  signal(SIGINT, exit_signal_handler);

  RobotCarParams robot_params(argc, argv);

  // Launch Robot Communications client
  wsClient client;
  client.connect(robot_params.uri);

  robot_car.Init(robot_params.Kp,
                 robot_params.Ki,
                 robot_params.Kd,
                 robot_params.max_val);

  while (true)
  {

    robot_car.ReadSensors();

    std::string sensor_data = robot_car.getSensorData();

    client.send_message(sensor_data);

    robot_car.UpdateOdometry();

    if (robot_car.EnterGreen())
    {
      robot_car.StopForSeconds(10);
      continue;
    }
    if (robot_car.ExitGreen())
    {
      robot_car.TurnRight();
    }
    if (robot_car.EnterOrange())
    {
      robot_car.TurnLeft();
    }
    if (robot_car.EnterPink())
    {
      robot_car.DriveStraight();
      //robot_car.TurnRight();
    }
    robot_car.Update();

    //robot_car.DriveCrazy();
  }
  client.join_thread();
}

void exit_signal_handler(int signo)
{
  if (signo == SIGINT)
  {
    robot_car.Stop();
    robot_car.Reset();
    exit(-2);
  }
}
