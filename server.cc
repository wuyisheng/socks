//
//  main.cc
//
//  Created by 吴怡生 on 1/03/2018.
//  Copyright © 2018 yeshen.org. All rights reserved.
//

#include "server.h"

#include <iostream>
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
#include <fcntl.h>
#include <sys/epoll.h>
#include <netdb.h>

#define SERVER_PORT 2018
#define MAX_CONNECTION 10
#define MAX_DATA_SIZE 1000
#define MAX_EVENT 32

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

    //create and bind
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

    int sock_flags = fcntl(sock_fd,F_GETFL,0);
    sock_flags |= O_NONBLOCK;
    if(fcntl(sock_fd, F_SETFL, sock_flags) == -1){
        perror("fail to set nonblock");
        exit(1);
    }

    //listen
    if(listen(sock_fd,MAX_CONNECTION) == -1){
        perror("fail to listen");
        exit(1);
    }

    int epoll_fd;
    if((epoll_fd = epoll_create1(0))==-1){
        perror("fail to create epoll");
        exit(1);
    }

    struct epoll_event event,events[MAX_EVENT];
    event.data.fd = sock_fd;
    event.events = EPOLLIN | EPOLLET;
    if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,sock_fd,&event) == -1){
        perror("fail to create epoll ctl");
        exit(1);
    }

    while(true){
        int n = epoll_wait(epoll_fd,events,MAX_EVENT,-1);
        for(int i =0;i<n; ++i){
            if(events[i].events & EPOLLERR || 
                events[i].events & EPOLLHUP ||
                !(events[i].events & EPOLLIN) ){
                    perror("error in epoll event");
                    close(events[i].data.fd);
            }else if(sock_fd == events[i].data.fd){
                while(true){
                    struct sockaddr in_addr;
                    socklen_t in_len = sizeof(in_addr);
                    int in_fd;
                    if((in_fd = accept(sock_fd,&in_addr,&in_len)) == -1){
                        if(errno == EAGAIN || errno == EWOULDBLOCK){
                            continue;
                        }else{
                            perror("accept failed");
                            continue;
                        }
                    }
                    std::string hbuf(NI_MAXHOST,'\0');
                    std::string sbuf(NI_MAXSERV,'\0');
                    if(getnameinfo(&in_addr,in_len,
                        const_cast<char*>(hbuf.data()),hbuf.size(),
                        const_cast<char*>(sbuf.data()),sbuf.size(),
                        NI_NUMERICHOST | NI_NUMERICSERV) == 0 ){
                            
                        std::cout << "[I] Accepted" << in_fd << "(host=" << hbuf << ", port=" << sbuf << ")" << endl;
                    }

                    int in_flags = fcntl(in_fd,F_GETFL,0);
                    in_flags |= O_NONBLOCK;
                    if(fcntl(in_fd, F_SETFL, in_flags) == -1){
                        std::cout << "nonblocking" << "(host=" << hbuf << endl;
                        continue;
                    }

                    event.data.fd = in_fd;
                    event.events = EPOLLIN | EPOLLET;
                    if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,in_fd,&event) ==-1){
                        printf("fail to add epoll");
                        continue;
                    }
                    if(send(in_fd,"Hello, you are connected!\n",26,0) == -1){
                        perror("fail to send");
                    }
                    break;
                }
            }else{
                int fd = events[i].data.fd;
                while(true){
                    char buf[512];
                    int count = read(fd,buf,512);
                    if(count == -1){
                        if(errno == EAGAIN){
                            continue;
                        }
                    }else if(count ==0){
                        printf("close %d\n",fd);
                        close(fd);
                        continue;
                    }
                    std::cout << fd << " says: " <<  buf;
                    break;
                }
            }
        }
    }
    close(sock_fd);
    // while(1){
    //     sin_size = sizeof(struct sockaddr_in);
    //     if((client_fd = accept(sock_fd,
    //         (struct sockaddr *)& remote_addr,
    //         (socklen_t*)&sin_size)) ==-1){
    //             perror("error occurs in accept");
    //             continue;
    //     }
    //     printf("connet:%s:%u\n",
    //         inet_ntoa(remote_addr.sin_addr),
    //         ntohs(remote_addr.sin_port));
    //     if(!fork()){
    //         if(send(client_fd,"Hello, you are connected!\n",26,0) == -1){
    //             perror("fail to send");
    //             close(client_fd);
    //             exit(0);
    //         }
    //     }
    //     close(client_fd);
    //}
}

void Server::stop(){
    cout << "stop" << endl;
}