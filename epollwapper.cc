//
//  epollwapper.cc
//
//  Created by 吴怡生 on 12/03/2018.
//  Copyright © 2018 yeshen.org. All rights reserved.
//

#include "epollwapper.h"
#include <inttypes.h>
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

#define MAX_EVENT 32

using namespace std;

EpollWapper::EpollWapper(){
    this->epoll_fd_ = -1;
}

int EpollWapper::create(EpollNotify* listener){
    struct epoll_event event = new epoll_event();
    struct epoll_event events[] = new epoll_event[MAX_EVENT]();
    this->event_ = &event;
    this->events_ = &events;
    int epoll_fd;
    if((epoll_fd = epoll_create1(0))==-1){
        perror("fail to create epoll");
        return -1;
    }
    this->epoll_fd_ = epoll_fd;
    this->listener_ = listener;
}

NetStatus EpollWapper::watchTcp(uint16_t port,int* fd,void* p){
    
}

NetStatus EpollWapper::watchUdp(uint16_t port,int* fd,void* p){

}

NetStatus EpollWapper::createTcp(uint16_t* port,int* fd,void* p){

}

NetStatus EpollWapper::createUdp(uint16_t* port,int* fd,void* p){

}

bool EpollWapper::remove(int fd){
    close(fd);
    return epoll_ctl(this->epoll_fd_,EPOLL_CTL_DEL,NULL) != -1;
}

void EpollWapper::wait(){
    if(this->epoll_fd_ < 0 || !this->listener_){
        throw "epoll not ready,call creat() before wait";
    }
    while(1){
        int count = epoll_wait(this->epoll_fd_,*this->events_,MAX_EVENT,-1);
        cout << "connect come" << endl;
        for(int i=0;i<count;i++){
            struct epoll_event item = (*this->events_)[i];
            int from = item.data.fd;
            void* ptr = item.data.ptr;
            if(item.events & EPOLLERR ||
                item.events & EPOLLHUP ||
                item.events & EPOLLIN){
                    perror("error in epoll event");
                    this->listener_.onDestory(ptr,from);
            }else if(false){
                //TODO accept new connection
            }else{
                LBuff buf = new LBuff(); 
                LBuff next = &buf;
                while(1){
                    int count = read(from,*next->buf,LBUFF_SIZE);
                    if(count == -1){
                        if(error == EAGAIN){
                            continue;
                        }else{
                            perror("read error");
                            break;
                        }
                    }else if(count == 0){
                        break;
                    }else if(count > 0){
                        LBuff tmp = new LBuff(); 
                        *next->next = &tmp;
                        *next = *next->next;
                    }
                }
                this->listener_.onData(&buf,ptr,from);
            }
        }
    }
}

bool EpollWapper::setNonblocking(int fd){
    int socket_flags = fcntl(fd,F_GETFL,0);
    socket_flags |= O_NONBLOCK;
    return fcntl(fd,F_SETFL,socket_flags) != -1;
}

bool EpollWapper::addIntoEpoll(int fd,void* ptr){
    struct  epoll_event event = (struct epoll_event)*this->event;
    event.data.fd = fd;
    event.data.ptr = ptr;
    event.events = EPOLLIN | EPOLLET;
    return epoll_ctl(this->epoll_fd_,EPOLL_CTL_ADD,fd,&event) != -1;
}

EpollWapper::~EpollWapper(){
    //TODO
}