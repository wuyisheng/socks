//
//  socks5.h
//
//  Created by 吴怡生 on 12/03/2018.
//  Copyright © 2018 yeshen.org. All rights reserved.
//

#ifndef SOCKS5_H
#define SOCKS5_H
#include <inttypes.h>

enum SectionStatus{
    INIT = 0,
    REQUEST = 1,
    REDY = 2,
    CLOSE = 3
};

struct UDPPair{
    int in_fd;
    int out_fd;
    UDPPair *next;
};

struct Section{
    int fd;
    bool tcp;
    SectionStatus status;
    int watch_fd;
    UDPPair *udp;
};

struct RequestVersion{
    uint8_t VER;
    uint8_t NMETHODS;
    uint8_t METHODS;
};

struct ReplytVersion{
    uint8_t VER;
    uint8_t METHOD;
};

struct Request{
    uint8_t VER;
    uint8_t CMD;
    uint8_t RSV;
    uint8_t ATYP;
    char* DST_ADDR;
    char* DST_PORT;
};

struct Replies{
    uint8_t VER;
    uint8_t REP;
    uint8_t RSV;
    uint8_t ATYP;
    char* BND_ADDR;
    char* BND_PORT;
};

namespace Socks5{
    void handleAccept();
    void handleRequest();
    void replyMethod();
    void replyRequestTcp();
    void replyReqestUdp();
    void forward();
};

#endif