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
#include "common.h"

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
    int sock_fd;
    struct sockaddr_in local_addr;
    struct sockaddr_in remote_adder;
    if((sock_fd == socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("fail to create socket");
        return NetStatus.FAIL;
    }
    long flag = 1;
    setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR,(char*)&flag,sizeof(flag));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(port);
    local_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(local_addr.sin_zero),8);
    if(!this->setNonblocking(sock_fd)){
        perror("fail to set nonblocking");
        return NetStatus.FAIL;
    }

    if(bind(sock_fd,(struct sockaddr *)&local_addr,sizeof(struct sockaddr)) == -1){
        perror("fail to bind"):
        return NetStatus.FAIL;
    }
    if(listen(sock_fd,MAX_CONNECTION) == -1){
        perror("fail to listen");
        return NetStatus.FAIL;
    }
    *fd = sock_fd;
    struct EpollData data = new EpollData();
    data.type = TYPE_WATCH | TYPE_TCP;
    data.ptr = p;
    return this->addIntoEpoll(sock_fd,&data) ? NetStatus.SUCC : NetStatus.FAIL;
}

NetStatus EpollWapper::watchUdp(uint16_t port,int* fd,void* p){
    int sock_fd;
    struct sockaddr_in local_addr;
    if((sock_fd=socket(AF_INET,SOCK_DGRAM,0))==-1){
        perror("fail to create socket udp"):
        return NetStatus.FAIL;
    }
    this->setNonblocking(sock_fd);
    bzero(&local_addr,sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    local_addr.sin_port = htons(port);
    if(bind(sock_fd,(struct sockaddr*)&local_addr,sizeof(local_addr)) == -1){
        perror("fail to bind udp");
        return NetStatus.FAIL;
    }
    *fd = sock_fd;
    struct EpollData data = new EpollData();
    data.type = TYPE_WATCH | TYPE_UDP;
    data.ptr = p;
    return this->addIntoEpoll(sock_fd,&data) ? NetStatus.SUCC : NetStatus.FAIL;
}

NetStatus EpollWapper::createTcp(char* target_ip, uint16_t target_port,int* fd,void* p){
    int sock_fd;
    struct sockaddr_in target;
    if((sock_fd == socket(AF_INET,SOCK_STREAM,0)) == -1){
        perror("fail to create socket tcp"):
        return NetStatus.FAIL;
    }
    this->setNonblocking(sock_fd);
    bzero((char*)&target,sizeof(target));
    target.sin_family = AF_INET;
    target.sin_addr.s_addr = inet_addr(target_ip);
    target.sin_port = htons(target_port);

    if(connect(sock_fd,(struct sockaddr*)&target,sizeof(target)) == -1){
        perror("fail to connect socket"):
        return NetStatus.FAIL;
    }
    *fd = sock_fd;
    struct EpollData data = new EpollData();
    data.type = TYPE_TCP;
    data.ptr = p;
    return this->addIntoEpoll(sock_fd,&data) ? NetStatus.SUCC : NetStatus.FAIL;
}

NetStatus EpollWapper::createUdp(char* target_ip, uint16_t target_port,int* fd,void* p){
    int sock_fd;
    struct sockaddr_in local_addr;
    if((sock_fd=socket(AF_INET,SOCK_DGRAM,0)) == -1){
        perror("fail to create socket udp"):
        return NetStatus.FAIL;
    }
    this->setNonblocking(sock_fd);
    bzero(&local_addr,sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = inet_addr(target_ip);
    local_addr.sin_port = htons(target_port);
    if(bind(sock_fd,(struct sockaddr*)local_addr,sizeof(local_addr)) == -1){
        perror("fail to bind socket udp"):
        return NetStatus.FAIL;
    }
    *fd = sock_fd;
    struct EpollData data = new EpollData();
    data.type = TYPE_UDP;
    data.ptr = p;
    return this->addIntoEpoll(sock_fd,&data) ? NetStatus.SUCC : NetStatus.FAIL;
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
                item.events & EPOLLIN ||
                !ptr){
                    perror("error in epoll event");
                    this->listener_.onDestory(ptr,from);
            }else{
                struct EpollData* data = (EpollData*)ptr;
                if(data->isWatch&TYPE_WATCH != 0){
                    if(data->isWatch&TYPE_TCP != 0){
                        this->acceptTcp(from,ptr);
                    }else{//udp
                        this->acceptUdp(from,ptr);
                    }
                }else if(data->isWatch&TYPE_TCP != 0){
                    this->handleTcpData(from,ptr);
                }else{//udp
                    this->handleUdpData(from,ptr);
                }
            }
        }
    }
}

void EpollWapper::handleTcpData(int fd,void* ptr){
    struct LinkBuff buf = new LinkBuff(); 
    struct LinkBuff next = &buf;
    while(1){
        int count = read(fd,*next->buf,LBUFF_SIZE);
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
            struct LinkBuff tmp = new LinkBuff(); 
            *next->next = &tmp;
            *next = *next->next;
        }
    }
    this->listener_.onData(&buf,ptr,from);
};

void EpollWapper::handleUdpData(int fd,void* ptr){
    struct LinkBuff buf = new LinkBuff();
    struct LinkBuff next = &buf;
    while(1){
        struct sockaddr_in client_addr;
        socklen_t addrlen = sizeof(client_addr);
        int count = recvfrom(fd,*next->buf,LBUFF_SIZE,0,(sockaddr*)&client_addr,&addrlen);
        if(count <= 0){
            if(errno == ECONNRESET){
                continue;
            }else{
                perror("read error");
                break;
            }
        }else{
            struct LinkBuff tmp = new LinkBuff();
            *next->next = &tmp;
            *netx = *next->next;
        }
    }
};

void EpollWapper::acceptTcp(int fd,void* ptr){
    while(1){
        struct sockaddr in_addr;
        socklen_t in_len = sizeof(in_addr);
        int in_fd;
        if((in_fd == accept(fd,&in_addr,&in_len))==-1){
            if(errno == EAGAIN || errno == EWOULDBLOCK){
                break;
            }else{
                perror("accept fail");
                break;
            }
        }
        string hbuf(NI_MAXHOST,'\0');
        string sbuf(NI_MAXSERV,'\0');
        if(getnameinfo(&in_addr,in_len,
            const_cast<char*>(hbuf.data()),hhuf.size(),
            const_cast<char*>(sbuf.data()),sbuf.size(),
            NI_NUMERICHOST || NI_NUMERICSERV
            ) == 0 ){

            cout << "Accepted:" <<in_fd << "(host=" <<hbuf << ",port=" << sbuf << ")" << endl;
        }
        if(!this->setNonblocking(in_fd)){
            perror("fail to set nonblock");
            break;
        }
        struct EpollData data = new EpollData();
        data.type = TYPE_TCP;
        this->listener_.onAccept(in_fd,data.type,&data.ptr);
        if(!this->addIntoEpoll(in_fd,&data)){
            perror("fail to add into epoll");
            break;
        }
        continue;
    }
};

void EpollWapper::acceptUdp(int fd,void* ptr){

};

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