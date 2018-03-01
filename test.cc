//
//  main.cc
//
//  Created by 吴怡生 on 25/02/2018.
//  Copyright © 2018 yeshen.org. All rights reserved.
//

#include <iostream>
#include "client.h"


int main(int argc, const char * argv[]) {
    Client c("127.0.0.1",1080);
    c.run();
}
