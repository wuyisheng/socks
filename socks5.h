//
//  socks5.h
//
//  Created by yeshen on 12/03/2018.
//  Copyright © 2018 yeshen.org. All rights reserved.
//

#ifndef SOCKS5_H
#define SOCKS5_H
#include <inttypes.h>
#include "common.h"

#define ACCEPT_VERSION '\x05'
#define NO_AUTHON '\x00'
#define AUTHON '\x02'

enum SectionStatus
{
  SS_INIT = 0,
  SS_REQUEST = 1,
  SS_REDY = 2,
  SS_CLOSE = 3
};

struct UDPPair
{
  int in_fd;
  int out_fd;
  UDPPair *next;
};

struct Section
{
  int fd;
  bool tcp;
  SectionStatus status;
  int watch_fd;
  UDPPair *udp;
  struct LinkBuff *insidebuff;
  struct LinkBuff *outsidebuff;
};

struct RequestVersion
{
  uint8_t VER;
  uint8_t NMETHODS;
  uint8_t METHODS;
};

struct ReplytVersion
{
  uint8_t VER;
  uint8_t METHOD;
};

struct Request
{
  uint8_t VER;
  uint8_t CMD;
  uint8_t RSV;
  uint8_t ATYP;
  char *DST_ADDR;
  uint16_t DST_PORT;
};

struct Replies
{
  uint8_t VER;
  uint8_t REP;
  uint8_t RSV;
  uint8_t ATYP;
  char *BND_ADDR;
  uint16_t BND_PORT;
};

namespace Socks5
{
  bool decodeRequestVersion(struct RequestVersion *version, struct LinkBuff *buf);
  bool decodeRequest(struct Request *version, struct LinkBuff *buf);
  uint8_t *createReplytVersion();
  uint8_t *createReplies(uint8_t rep);
};

#endif