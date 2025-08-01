//
//  server.h
//
//  Created by yeshen on 1/03/2018.
//  Copyright © 2018 yeshen.org. All rights reserved.
//

#ifndef SERVER_H
#define SERVER_H

class Server
{
private:
    int port;
    bool running; 
public:
    Server();
    Server(int port);
    void start();
    void stop();
};

#endif
