//
//  client.cc
//
//  Created by yeshen on 1/03/2018.
//  Copyright © 2018 yeshen.org. All rights reserved.
//

#include <iostream>
#include <string>
#include "client.h"

#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_DATA_SIZE 1000
#define LOCAL_PORT "6006"

using namespace std;

Client::Client(const char *ip, const char *port)
{
  this->ip = ip;
  this->port = port;
};

void Client::run()
{
  cout << "run" << this->ip << this->port << endl;
  int sock_fd = -1;
  char buf[MAX_DATA_SIZE];
  int recvbytes, sendbytes, len;

  in_addr_t server_ip = inet_addr(this->ip);
  in_port_t server_port = atoi(this->port);
  in_addr_t my_ip = inet_addr(this->ip);
  in_port_t my_port = atoi(LOCAL_PORT);

  if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("fail to creat socket");
    exit(1);
  }

  long flag = 1;
  setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(flag));

  struct sockaddr_in my_addr;
  my_addr.sin_addr.s_addr = my_ip;
  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(my_port);

  if (bind(sock_fd, (sockaddr *)&my_addr, sizeof(sockaddr)) == -1)
  {
    perror("fail to bind socket");
    exit(1);
  }

  struct sockaddr_in server_addr;
  server_addr.sin_addr.s_addr = server_ip;
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(server_port);
  printf("try to connect %s:%u\n",
         inet_ntoa(server_addr.sin_addr),
         ntohs(server_addr.sin_port));

  if (connect(sock_fd,
              (struct sockaddr *)&server_addr,
              sizeof(struct sockaddr)) == -1)
  {
    perror("fail to connect");
    exit(1);
  }

  if (send(sock_fd, "here\n", 4, 0) == -1)
  {
    perror("fail to send");
  }

  if ((recvbytes = recv(sock_fd, buf, MAX_DATA_SIZE, 0)) == -1)
  {
    perror("fail to recv");
    exit(1);
  }
  buf[recvbytes] = '\0';
  printf("Received: %s", buf);

  close(sock_fd);
  exit(0);
}
