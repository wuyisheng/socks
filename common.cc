//
//  common.cc
//
//  Created by yeshen on 14/03/2018.
//  Copyright © 2018 yeshen.org. All rights reserved.
//

#include "common.h"
#include <stdio.h>
#include <stdlib.h>

namespace LinkBuffUtil
{
  void dofree(struct LinkBuff *ptr)
  {
    struct LinkBuff *next = ptr;
    struct LinkBuff *tmp;
    while (next)
    {
      tmp = next->next;
      free(next->buf);
      delete next;
      next = tmp;
    }
  };
}