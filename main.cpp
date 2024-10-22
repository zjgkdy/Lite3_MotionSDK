#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 43893
#define BUFFER_SIZE 1024

int main()
{
  int sockfd;
  char buffer[BUFFER_SIZE];
  struct sockaddr_in server_addr, client_addr;
  socklen_t addr_len = sizeof(client_addr);

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
  server_addr.sin_addr.s_addr = INADDR_ANY; // 绑定到所有本地接口
  server_addr.sin_port = htons(PORT);

  // 绑定Socket到地址
  if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
  {
    perror("Bind failed");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  std::cout << "UDP Server is listening on port " << PORT << std::endl;

  while (true)
  {
    // 接受消息
    int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &addr_len);
    std::cout << client_addr.sin_port << std::endl;
    // std::cout << "Client: " << buffer << std::endl;

    // 响应消息
    uint32_t response = 0x0909;
    sendto(sockfd, &response, 4, 0, (const struct sockaddr *)&client_addr, addr_len);
  }

  close(sockfd);
  return 0;
}