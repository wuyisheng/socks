//
//  socks5server.cc
//
//  Created by 吴怡生 on 13/03/2018.
//  Copyright © 2018 yeshen.org. All rights reserved.
//

#include <iostream>
#include <inttypes.h>
#include "socks5server.h"
#include "epollwapper.h"
#include "socks5.h"

using namespace std;

Socks5Server::Socks5Server(int port){
    this->port = port;
};

void Socks5Server::forever(){
    cout << "test" << endl;
};

void Socks5Server::onData(LBuff* buf,void* ptr,int fd){
    cout << "onData" << endl;
};

void Socks5Server::onDestory(void* data,int fd){
    cout << "onDestory" << endl;
};



int main(int argc, const char * argv[]) {
    Socks5Server server(1080);
    server.forever();
}