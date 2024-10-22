#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "dr_timer.h"
#include "cmd_structure.h"

#define PORT 43893
#define BUFFER_SIZE 1024
#define IP "192.168.0.36" // 服务器地址

int main()
{
  int sockfd;
  char buffer[BUFFER_SIZE];
  struct sockaddr_in server_addr;

  /* 定时器初始化 */
  double now_time, start_time;
  DRTimer set_timer;
  set_timer.TimeInit(500); // 定时器周期：ms

  // 创建UDP Socket
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0)
  {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }

  // 配置服务器地址
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);

  // 使用inet_pton将IP地址从字符串转换为网络字节顺序
  if (inet_pton(AF_INET, IP, &server_addr.sin_addr) <= 0)
  {
    std::cerr << "Invalid address/ Addresss not supported" << std::endl;
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  start_time = set_timer.GetCurrentTime(); // 获取当前时间
  while (true)
  {

    // 发送消息到服务器
    if (set_timer.TimerInterrupt() == true) // 等待定时器触发
    {
      continue;
    }

    // 发送消息
    struct CommandHead command_head = {0};
    command_head.code = INIT_CODE;
    command_head.type = INIT_TYPE;
    int send_result = sendto(sockfd, &command_head, sizeof(command_head), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (send_result < 0)
    {
      std::cerr << "Sento failed: " << strerror(errno) << std::endl;
      break;
    }
  }

  close(sockfd);
  return 0;
}