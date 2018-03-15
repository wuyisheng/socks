//
//  socks5.cc
//
//  Created by 吴怡生 on 12/03/2018.
//  Copyright © 2018 yeshen.org. All rights reserved.
//

#include <inttypes.h>
#include <stdlib.h>
#include <netdb.h>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include "socks5.h"
#include "common.h"

namespace Socks5{
    bool decodeRequestVersion(struct RequestVersion* version,struct LinkBuff* buf){
        if(!version || !buf) return false;
        version->VER = buf->buf[0];
        version->NMETHODS = buf->buf[1];
        version->METHODS = buf->buf[2];
        return true;
    };

    /*
     The SOCKS request is formed as follows:

        +----+-----+-------+------+----------+----------+
        |VER | CMD |  RSV  | ATYP | DST.ADDR | DST.PORT |
        +----+-----+-------+------+----------+----------+
        | 1  |  1  | X'00' |  1   | Variable |    2     |
        +----+-----+-------+------+----------+----------+

     Where:

          o  VER    protocol version: X'05'
          o  CMD
             o  CONNECT X'01'
             o  BIND X'02'
             o  UDP ASSOCIATE X'03'
          o  RSV    RESERVED
          o  ATYP   address type of following address
             o  IP V4 address: X'01'
             o  DOMAINNAME: X'03'
             o  IP V6 address: X'04'
          o  DST.ADDR       desired destination address
          o  DST.PORT desired destination port in network octet
             order
    */
    bool decodeRequest(struct Request* request,struct LinkBuff* buf){
        if(!request || !buf)return false;
        uint8_t* buffer = buf->buf;
        request->VER = buffer[0];
        request->CMD = buffer[1];
        request->RSV = buffer[2];
        request->ATYP = buffer[3];
        if(request->ATYP == '\x01'){//IP V4 address
            in_addr temp;
            char *tmpbuf = (char *)&temp;
            tmpbuf[0]=buffer[4];
            tmpbuf[1]=buffer[5];
            tmpbuf[2]=buffer[6];
            tmpbuf[3]=buffer[7];
            request->DST_ADDR = inet_ntoa(*(struct in_addr*)&temp);
            request->DST_PORT = ((uint16_t)buffer[8] << 8) + (uint16_t)buffer[9];
        }else if(request->ATYP == '\x03'){//DOMAINNAME
            uint8_t len = buffer[4];
            char temp[len];
            strncpy(temp,(char *)buffer+5,len);
            request->DST_ADDR=temp;
            request->DST_PORT = ((uint16_t)buffer[len+5] << 8) + (uint16_t)buffer[len+5+1];
        }else if(request->ATYP == '\x04'){//IP V6 address
            //unsupport
            return false;
        }
        return true;
    };

    uint8_t* createReplytVersion(){
        uint8_t *buf=(uint8_t*)malloc(2); 
        buf[0] = '\x05';
        buf[1] = '\x00';
        return buf;
    };


    /*
    The SOCKS request information is sent by the client as soon as it has
    established a connection to the SOCKS server, and completed the
    authentication negotiations.  The server evaluates the request, and
    returns a reply formed as follows:

            +----+-----+-------+------+----------+----------+
            |VER | REP |  RSV  | ATYP | BND.ADDR | BND.PORT |
            +----+-----+-------+------+----------+----------+
            | 1  |  1  | X'00' |  1   | Variable |    2     |
            +----+-----+-------+------+----------+----------+

        Where:

            o  VER    protocol version: X'05'
            o  REP    Reply field:
                o  X'00' succeeded
                o  X'01' general SOCKS server failure
                o  X'02' connection not allowed by ruleset
                o  X'03' Network unreachable
                o  X'04' Host unreachable
                o  X'05' Connection refused
                o  X'06' TTL expired
                o  X'07' Command not supported
                o  X'08' Address type not supported
                o  X'09' to X'FF' unassigned
            o  RSV    RESERVED
            o  ATYP   address type of following address
            o  IP V4 address: X'01'
            o  DOMAINNAME: X'03'
            o  IP V6 address: X'04'
            o  BND.ADDR       server bound address
            o  BND.PORT       server bound port in network octet order

    Fields marked RESERVED (RSV) must be set to X'00'.
    */
    uint8_t* createReplies(uint8_t rep){
        uint8_t *buf=(uint8_t*)malloc(10); 
        buf[0] = '\x05';
        buf[1] = rep ;
        buf[2] = '\x00';
        buf[3] = '\x01';
        buf[4] = '\xC0';
        buf[5] = '\xA8';
        buf[6] = '\x00';
        buf[7] = '\x5C';
        buf[8] = '\x27';
        buf[9] = '\x24';
        return buf;
    };
}
