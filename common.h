//
//  common.h
//
//  Created by yeshen on 14/03/2018.
//  Copyright © 2018 yeshen.org. All rights reserved.
//

#ifndef COMMON_H
#define COMMON_H

#include <inttypes.h>

#define LBUFF_SIZE 512

struct LinkBuff
{
  uint8_t *buf;
  int size;
  LinkBuff *next;
};

namespace LinkBuffUtil
{
  void dofree(struct LinkBuff *ptr);
};

#endif