//
//  client.h
//
//  Created by yeshen on 1/03/2018.
//  Copyright © 2018 yeshen.org. All rights reserved.
//

#include <string>

#ifndef CLIENT_H
#define CLIENT_H

class Client
{
private:
  const char *port;
  const char *ip;

public:
  Client(const char *ip, const char *port);
  void run();
};

#endif