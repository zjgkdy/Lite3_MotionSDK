/// @file main.cpp
/// @author ShuyangYu
/// @brief
/// @version 1.0
/// @date 2024-10-19
/// @copyright Copyright (c) 2024

#include "udpsocket.hpp"
#include "udpserver.hpp"
#include "sender.h"
#include "dr_timer.h"
#include "receiver.h"
#include "motionexample.h"
#include <iostream>
#include <time.h>
#include <string.h>

using namespace std;

bool is_message_updated_ = false; ///< Flag to check if message has been updated
/**
 * @brief Callback function to set message update flag
 *
 * @param code The code indicating the type of message received
 */
void OnMessageUpdate(uint32_t code)
{
  if (code == 0x0906)
  {
    is_message_updated_ = true;
  }
}

int main(int argc, char *argv[])
{
  /* 定时器初始化 */
  double now_time, start_time;
  DRTimer set_timer;
  set_timer.TimeInit(1); // 定时器周期：ms

  /* 网络通讯初始化 */
  Sender *send_cmd = new Sender("192.168.1.120", 43893); // 创建Sender：绑定机器人的IP和端口号
  Receiver *robot_data_recv = new Receiver();            // 创建Receiver
  robot_data_recv->RegisterCallBack(OnMessageUpdate);
  robot_data_recv->StartWork();

  /* 机器人初始化 */
  send_cmd->RobotStateInit(); // 机器人关节状态归零位，获取机器人控制权

  /* Motion初始化 */
  MotionExample robot_set_up_demo;
  start_time = set_timer.GetCurrentTime(); // 获取当前时间
  RobotData *robot_data = &robot_data_recv->GetState();
  robot_set_up_demo.GetInitData(robot_data->joint_data, 0.000); // 获取所有的关节数据

  RobotCmd robot_joint_cmd; // 机器人控制命令
  memset(&robot_joint_cmd, 0, sizeof(robot_joint_cmd));

  /* 主循环 */
  int time_tick = 0;
  while (1)
  {
    if (set_timer.TimerInterrupt() == true) // 等待定时器触发
    {
      continue;
    }

    now_time = set_timer.GetIntervalTime(start_time); // 计算相对时间
    time_tick++;

    if (time_tick < 1000)
    {
      robot_set_up_demo.PreStandUp(robot_joint_cmd, now_time, *robot_data); ///< Stand up and prepare for action
    }
    if (time_tick == 1000)
    {
      robot_set_up_demo.GetInitData(robot_data->joint_data, now_time); ///< Obtain all joint states once before each stage (action)
    }

    if (time_tick >= 1000)
    {
      robot_set_up_demo.StandUp(robot_joint_cmd, now_time, *robot_data); ///< Full stand up
    }

    if (time_tick >= 10000)
    {
      send_cmd->ControlGet(ROBOT); // 将控制权归还ROBOT(机器人的原始控制算法)
      break;
    }

    if (is_message_updated_)
    {
      // send_cmd->SendCmd(robot_joint_cmd);
    }

    cout << robot_data->joint_data.hl_leg->torque << endl;
  }

  return 0;
}
