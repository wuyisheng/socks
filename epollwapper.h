//
//  epollwapper.h
//
//  Created by 吴怡生 on 12/03/2018.
//  Copyright © 2018 yeshen.org. All rights reserved.
//

#ifndef EPOLL_WAPPER_H
#define EPOLL_WAPPER_H

#include <sys/epoll.h>
#include <inttypes.h>
#include "common.h"

#define MAX_CONNECTION 10

#define TYPE_WATCH  1
#define TYPE_TCP    2
#define TYPE_UDP    4

enum NetStatus{
    NET_SUCC = '\x00',      //succeeded
    NET_FAIL = '\x01',      //general SOCKS server failure
    NET_NOTALLOW = '\x02',  //connection not allowed by ruleset
    NET_NUREACH = '\x03',   //Network unreachable
    NET_HUREACH = '\x04',   //Host unreachable
    NET_CREFUSH = '\x05',   //Connection refused
    NET_TTL = '\x06',       //TTL expired
    NET_COMMAND = '\x07',   //Command not supported
    NET_ADRESS = '\x08'     //Address type not supported
};

struct EpollData{
    uint8_t type;
    void* ptr;
};

class EpollNotify{
    public:
        virtual void onAccept(int fd,int type,void* ptr) = 0;
        virtual void onData(struct LinkBuff* buf,void* ptr,int fd) = 0;
        virtual void onDestory(void* data,int fd) = 0;
};

class EpollWapper{
    public:
        EpollWapper();
        int create(EpollNotify* listener);
        NetStatus watchTcp(uint16_t port,int* fd,void* ptr);
        NetStatus watchUdp(uint16_t port,int* fd,void* ptr);
        NetStatus createTcp(char* target_ip, uint16_t target_port,int* fd,void* ptr);
        NetStatus createUdp(char* target_ip, uint16_t target_port,int* fd,void* ptr);
        bool remove(int fd);
        void wait();
        ~EpollWapper();
    private:
        EpollNotify* listener_;
        struct epoll_event* event_;
        struct epoll_event* events_;
        int epoll_fd_;
        bool setNonblocking(int fd);
        bool addIntoEpoll(int fd,void* ptr);
        void handleTcpData(int fd,void* ptr);
        void handleUdpData(int fd,void* ptr);
        void acceptTcp(int fd,void* ptr);
        void acceptUdp(int fd,void* ptr);
};

#endif