//
//  client.cc
//
//  Created by 吴怡生 on 1/03/2018.
//  Copyright © 2018 yeshen.org. All rights reserved.
//

#include <iostream>
#include <string>
#include "client.h"

using namespace std;

Client::Client(){
    this->ip = "127.0.0.1";
    this->port = 1080;
};

Client::Client(std::string ip, int port){
    this->ip = ip;
    this->port = port;
};

void Client::run(){
    cout << "run" << this->ip << this->port << endl ;
}
