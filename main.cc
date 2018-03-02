//
//  main.cc
//
//  Created by yeshen on 25/02/2018.
//  Copyright © 2018 yeshen.org. All rights reserved.
//

#include <iostream>
#include "server.h"


int main(int argc, const char * argv[]) {
    Server h(1080);
    h.start();
    
}
