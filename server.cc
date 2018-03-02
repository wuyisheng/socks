//
//  main.cc
//
//  Created by yeshen on 1/03/2018.
//  Copyright © 2018 yeshen.org. All rights reserved.
//

#include <iostream>
#include "server.h"
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_PORT 2018
#define MAX_CONNECTION 10
#define MAX_DATA_SIZE 1000

using namespace std;

Server::Server(){
    this->port = 2018;
    this->running = false;
};

Server::Server(int port){
    this->port = port;
    this->running = false;
};

void Server::start(){
    cout << "start" << endl;

    int sock_fd,client_fd;
    int sin_size;
    struct sockaddr_in local_addr;
    struct sockaddr_in remote_addr;

    if((sock_fd = socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("fail to create socket");
        exit(1);
    }
    long flag = 1;
    setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR,(char *)&flag,sizeof(flag));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(SERVER_PORT);
    local_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(local_addr.sin_zero),8);

    if(bind(sock_fd,(struct sockaddr *)&local_addr,sizeof(struct sockaddr)) == -1){
        perror("fail to bind socket");
        exit(1);
    }

    if(listen(sock_fd,MAX_CONNECTION) == -1){
        perror("fail to listen");
        exit(1);
    }
    while(1){
        sin_size = sizeof(struct sockaddr_in);
        if((client_fd = accept(sock_fd,
            (struct sockaddr *)& remote_addr,
            (socklen_t*)&sin_size)) ==-1){
                perror("error occurs in accept");
                continue;
        }
        printf("connet:%s:%u\n",
            inet_ntoa(remote_addr.sin_addr),
            ntohs(remote_addr.sin_port));
        if(!fork()){
            if(send(client_fd,"Hello, you are connected!\n",26,0) == -1){
                perror("fail to send");
                close(client_fd);
                exit(0);
            }
        }
        close(client_fd);
    }
}

void Server::stop(){
    cout << "stop" << endl;
}
