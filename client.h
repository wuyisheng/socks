//
//  client.h
//
//  Created by 吴怡生 on 1/03/2018.
//  Copyright © 2018 yeshen.org. All rights reserved.
//

#include <string>

#ifndef CLIENT_H
#define CLIENT_H

using namespace std;

class Client
{
private:
    int port;
    std::string ip; 
public:
    Client();
    Client(std::string ip,int port);
    void run();
};

#endif