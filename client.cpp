#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "dr_timer.h"
#include "cmd_structure.h"

#define BUFFER_SIZE 1024
#define LOCAL_PORT 43897  // 本地端口
#define SERVER_PORT 43893 // 服务器端口
// #define IP "127.0.0.1"    // 本地回环地址
// #define IP "192.168.0.36" // 测试服务器地址
#define IP "192.168.1.120" // 运动主机服务器地址

struct RobotStateUpload *RobotState; // 机器人状态信息
struct RobotJointAngle *JointAngle;  // 关节角度
struct RobotJointVel *JointVel;      // 关节角速度
char buffer[BUFFER_SIZE];

template <typename T, std::size_t N>
std::string array_to_string(T (&arr)[N])
{
  std::string s = "(";
  for (std::size_t i = 0; i < N; ++i)
  {
    s += std::to_string(arr[i]);
    if (i < N - 1)
    {
      s += " ";
    }
  }
  s += ")";
  return s;
}

int main()
{
  /* 定时器初始化 */
  double now_time, start_time;
  DRTimer set_timer;
  set_timer.TimeInit(500); // 定时器周期：ms

  // 创建UDP Socket
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0)
  {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }

  // 绑定到指定端口
  sockaddr_in local_addr;
  local_addr.sin_family = AF_INET;
  local_addr.sin_addr.s_addr = INADDR_ANY; // 绑定到所有本地接口
  local_addr.sin_port = htons(LOCAL_PORT); // 指定端口
  if (bind(sockfd, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0)
  {
    std::cerr << "Bind failed" << std::endl;
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  // 配置服务器地址
  struct sockaddr_in server_addr;
  socklen_t addr_len = sizeof(server_addr);
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(SERVER_PORT);

  // 使用inet_pton将IP地址从字符串转换为网络字节顺序
  if (inet_pton(AF_INET, IP, &server_addr.sin_addr) <= 0)
  {
    std::cerr << "Invalid address/ Addresss not supported" << std::endl;
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  // 发送回零命令
  struct CommandHead command_head = {0};
  command_head.code = INIT_CODE;
  command_head.type = INIT_TYPE;
  int send_result = sendto(sockfd, &command_head, sizeof(command_head), 0, (struct sockaddr *)&server_addr, addr_len);
  if (send_result < 0)
  {
    std::cerr << "Sento failed: " << strerror(errno) << std::endl;
  }

  start_time = set_timer.GetCurrentTime(); // 获取当前时间
  while (true)
  {
    // 发送消息到服务器
    if (set_timer.TimerInterrupt() == true) // 等待定时器触发
    {
      continue;
    }

    // 接受数据测试
    std::cout << "Wait receive message!" << std::endl;
    int recv_size = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, &addr_len);
    std::cout << "Receive message!" << std::endl;
    CommandHead *head = (CommandHead *)buffer; // 解析指令头
    if (head->code == ROBOT_STATE_CODE)
    {
      RobotState = (struct RobotStateUpload *)(buffer + sizeof(CommandHead));
      std::cout << "robot_basic_state: " << RobotState->robot_basic_state << std::endl;
      std::cout << "robot_gait_state: " << RobotState->robot_gait_state << std::endl;
      std::cout << "(roll, pitch, yaw): " << array_to_string(RobotState->rpy) << std::endl;
      std::cout << "(roll_vel, pitch_vel, yaw_vel): " << array_to_string(RobotState->rpy_vel) << std::endl;
      std::cout << "(x_acc, y_acc, z_acc): " << array_to_string(RobotState->xyz_acc) << std::endl;
      std::cout << "pos_world: " << array_to_string(RobotState->pos_world) << std::endl;
      std::cout << "vel_world: " << array_to_string(RobotState->vel_world) << std::endl;
      std::cout << "vel_body: " << array_to_string(RobotState->vel_body) << std::endl;
      std::cout << "robot_motion_state: " << RobotState->robot_motion_state << std::endl;
      std::cout << "battery_level: " << RobotState->battery_level << std::endl;
      std::cout << "is_robot_need_move: " << RobotState->is_robot_need_move << std::endl;
      std::cout << "zero_position_flag: " << RobotState->zero_position_flag << std::endl;
      std::cout << "(forward_distance, backward_distance): " << array_to_string(RobotState->ultrasound) << std::endl;
    }
    else if (head->code == JOINT_ANGLE_CODE)
    {
      JointAngle = (struct RobotJointAngle *)(buffer + sizeof(CommandHead));
      std::cout << "joint_angle = " << array_to_string(JointAngle->joint_angle) << std::endl;
    }
    else if (head->code == JOINT_VELOCITY_CODE)
    {
      JointVel = (struct RobotJointVel *)(buffer + sizeof(CommandHead));
      std::cout << "joint_vel = " << array_to_string(JointVel->joint_vel) << std::endl;
    }
  }

  close(sockfd);
  return 0;
}