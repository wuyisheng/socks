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
        int port;
    public:
        Socks5Server(int port);
        void forever();
        virtual void onAccept(int fd,int type,void* ptr);
        virtual void onData(LBuff* buf,void* ptr,int fd);
        virtual void onDestory(void* data,int fd);
};

#endif