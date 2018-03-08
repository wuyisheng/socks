//
//  server.h
//
//  Created by 吴怡生 on 1/03/2018.
//  Copyright © 2018 yeshen.org. All rights reserved.
//

#ifndef SERVER_H
#define SERVER_H

#include <map>
#include <sys/epoll.h>

using namespace std;

class Section
{
private:
    int status;  //SectionStatus
    char * host;
    char * port;
    char cmd;
public:
    int inner;
    int outter;
    Section(int inner_fd);
    bool ready();
    bool handshake();
    char connet();
    bool forward(int from);
    void destory();
};

class SectionPool
{
public:
    map<int,Section*> data;
    SectionPool();
    void put(Section* section);
    void update(int fd,Section* section);
    void remove(Section* section);
    void remove(int fd);
    map<int, Section*>::iterator find(int fd);
};

class Server
{
private:
    SectionPool pool;
    struct epoll_event* event;
    int epoll_fd;
    int watch_port(int port);
    bool set_nonblocking(int fd);
    bool add_into_epoll(int sock_fd);
    bool accept_connect(int sock_fd);
    bool close_connect(int fd);
    void handle(int from);
public:
    Server();
    void forever();
};

#endif