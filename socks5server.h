//
//  socks5server.h
//
//  Created by 吴怡生 on 13/03/2018.
//  Copyright © 2018 yeshen.org. All rights reserved.
//

#ifndef SOCKS5_SERVER_H
#define SOCKS5_SERVER_H

#include "epollwapper.h"
#include <inttypes.h>

class Socks5Server: public EpollNotify{
    private:
        EpollWapper* wapper_;
    public:
        Socks5Server();
        void forever(uint16_t port);
        virtual void onAccept(int fd,int type,void* ptr);
        virtual void onData(struct LinkBuff* buf,void* ptr,int fd);
        virtual void onWritable(void* ptr,int fd);
        virtual void onDestory(void* data,int fd);
};

#endif