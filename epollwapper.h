//
//  epollwapper.h
//
//  Created by 吴怡生 on 12/03/2018.
//  Copyright © 2018 yeshen.org. All rights reserved.
//

#ifndef EPOLL_WAPPER_H
#define EPOLL_WAPPER_H

#include <inttypes.h>

enum NetStatus{
    SUCC = '\x00',      //succeeded
    FAIL = '\x01',      //general SOCKS server failure
    NOTALLOW = '\x02',  //connection not allowed by ruleset
    NUREACH = '\x03',   //Network unreachable
    HUREACH = '\x04',   //Host unreachable
    CREFUSH = '\x05',   //Connection refused
    TTL = '\x06',       //TTL expired
    COMMAND = '\x07',   //Command not supported
    ADRESS = '\x08'     //Address type not supported
};

class EpollNotify{
    public:
        virtual void onData(uint8_t* buf[],void* data,int fd) = 0;
        virtual void onDestory(void* data,int fd) = 0;
};

class EpollWapper{
    public:
        EpollWapper();
        void setListener(EpollNotify* listener);
        NetStatus watchTcp(uint16_t port);
        NetStatus watchUdp(uint16_t* port);
        NetStatus createTcp(uint16_t* port);
        NetStatus createUdp(uint16_t* port);
        ~EpollWapper();
    private:
        EpollNotify* listener_;
};

#endif