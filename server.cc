//
//  server.cc
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

#define SERVER_PORT 3018
#define MAX_CONNECTION 10
#define MAX_DATA_SIZE 1000
#define MAX_EVENT 32

using namespace std;

Section::Section(int inner_fd){
    this->status = 0;
    this->inner = inner_fd;
    this->outter = -1;
};

bool Section::ready(){

};

void Section::handshak(){

};

int Section::connet(){

};

void Section::forward(int from){
    
};
/*---section end---*/

SectionPool::SectionPool(){};

void SectionPool::put(Section section){
    //TODO
};

void SectionPool::remove(Section section){
    //TODO
};

void SectionPool::remove(int fd){
    //TODO
};

Section SectionPool::get(int fd){
    //TODO
};


/*----section pool end------*/

Server::Server(){};

bool Server::set_nonblocking(int fd){
    int sock_flags = fcntl(fd,F_GETFL,0);
    sock_flags |= O_NONBLOCK;
    return fcntl(fd, F_SETFL, sock_flags) != -1;
};

int Server::watch_port(int port){
    int sock_fd = -1;
    struct sockaddr_in local_addr;
    struct sockaddr_in remote_addr;
    if((sock_fd = socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("fail to create socket");
        exit(1);
    }
    long flag = 1;
    setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR,(char *)&flag,sizeof(flag));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(port);
    local_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(local_addr.sin_zero),8);

    if(bind(sock_fd,(struct sockaddr *)&local_addr,sizeof(struct sockaddr)) == -1){
        perror("fail to bind socket");
        exit(1);
    }
    if(!this->set_nonblocking(sock_fd)){
        perror("fail to set nonblock");
        exit(1);
    }
    if(listen(sock_fd,MAX_CONNECTION) == -1){
        perror("fail to listen");
        exit(1);
    }
    return sock_fd;
};

bool Server::add_into_epoll(struct epoll_event& event,int epoll_fd,int sock_fd){
    event.data.fd = sock_fd;
    event.events = EPOLLIN | EPOLLET;
    return epoll_ctl(epoll_fd,EPOLL_CTL_ADD,sock_fd,&event) != -1;
};

bool Server::accept_connect(struct epoll_event& event,int epoll_fd,int sock_fd){
    struct sockaddr in_addr;
    socklen_t in_len = sizeof(in_addr);
    int in_fd;
    if((in_fd = accept(sock_fd,&in_addr,&in_len)) == -1){
        if(errno == EAGAIN || errno == EWOULDBLOCK){
            return false;
        }else{
            perror("accept failed");
            return false;
        }
    }

    std::string hbuf(NI_MAXHOST,'\0');
    std::string sbuf(NI_MAXSERV,'\0');
    if(getnameinfo(&in_addr,in_len,
        const_cast<char*>(hbuf.data()),hbuf.size(),
        const_cast<char*>(sbuf.data()),sbuf.size(),
        NI_NUMERICHOST | NI_NUMERICSERV) == 0 ){
            
        std::cout << "Accepted:" << in_fd << "(host=" << hbuf << ", port=" << sbuf << ")" << endl;
    }

    if(!this->set_nonblocking(in_fd)){
        std::cout << "nonblocking" << "(host=" << hbuf << endl;
        return false;
    }
    if(!this->add_into_epoll(event,epoll_fd,in_fd)){
        printf("fail to add epoll\n");
        return false;
    }

    //TODO add into pool

    if(send(in_fd,"Hello, you are connected!\n",26,0) == -1){
        printf("fail to send\n");
        return false;
    }
    return true;
};

void Server::handle(int from){
    while(true){
        char buf[512];
        int count = read(from,buf,512);
        if(count == -1){
            if(errno == EAGAIN){
                continue;
            }
        }else if(count ==0){
            printf("close %d\n",from);
            close(from);
            continue;
        }else{
            std::cout <<"says:" <<  buf << endl;
        }
        break;
    }
};

void Server::forever(){
    cout << "start" << endl;

    int epoll_fd;
    struct epoll_event event,events[MAX_EVENT];
    int sock_fd = this->watch_port(SERVER_PORT);
    if((epoll_fd = epoll_create1(0))==-1){
        perror("fail to create epoll");
        exit(1);
    }
    if(!this->add_into_epoll(event,epoll_fd,sock_fd)){
        perror("fail to add into epoll");
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
                while(this->accept_connect(event,epoll_fd,sock_fd)){ /*keep empty*/}
            }else{
                this->handle(events[i].data.fd);
            }
        }
    }
    close(sock_fd);
};