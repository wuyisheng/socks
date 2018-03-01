//
//  main.cc
//
//  Created by 吴怡生 on 1/03/2018.
//  Copyright © 2018 yeshen.org. All rights reserved.
//

#include <iostream>
#include "server.h"

Server::Server(){
    this->port = 1080;
    this->running = false;
};

Server::Server(int port){
    this->port = port;
    this->running = false;
};

void Server::start(){
    using namespace std;
    cout << "start" << endl;
}


void Server::stop(){
    using namespace std;
    cout << "stop" << endl;
}
