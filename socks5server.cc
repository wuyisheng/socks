//
//  socks5server.cc
//
//  Created by 吴怡生 on 13/03/2018.
//  Copyright © 2018 yeshen.org. All rights reserved.
//

#include <iostream>
#include <inttypes.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "socks5server.h"
#include "epollwapper.h"
#include "socks5.h"

using namespace std;

Socks5Server::Socks5Server(){
    EpollWapper wapper = new EpollWapper();
    wapper.create(this);
    this->wapper_ = &wapper;
};

void Socks5Server::forever(uint8_t port){
    cout << "test" << endl;
    int fd;
    *this->wapper_->watchTcp(port,&fd,NULL);
    *this->wapper_->wait();
};

void Socks5Server::onAccept(int fd,int type,void* ptr){
    struct Section section =  new Section();
    section.fd = fd;
    section.tcp = type & TYPE_TCP != 0;
    section.status = SectionStatus.INIT;
    section.watch_fd = -1;
    *ptr = &section;
}

void Socks5Server::onDestory(void* ptr,int fd){
    if(!ptr || fd < 0)return;
    *(this->wapper_)->remove(fd);
    struct Section* section = (struct Section*)ptr;
    if(*section->watch_fd > 0){
        *(this->wapper_)->remove(*section->watch_fd);
    }
    struct UDPPair* forwardlist = *section->udp;
    while(forwardlist){
        *(this->wapper_)->remove(*forwardlist.in_fd);
        *(this->wapper_)->remove(*forwardlist.out_fd);
        forwardlist = *forwardlist.next;
    }
    delete *section;
};

void Socks5Server::onData(struct LBuff* buf,void* ptr,int fd){
    if(!ptr || !buf ) return;
    struct Section* section = (struct Section*)ptr;
    switch(*section->status){
        case SectionStatus.INIT:{
            struct RequestVersion version = new RequestVersion();
            if(!decodeRequestVersion(&version,buf)){
                perror("decode error");
                *(this->wapper_)->remove(fd);
                return;
            }
            if(version.VER != ACCEPT_VERSION){
                *(this->wapper_)->remove(fd);
                return;
            }
            uint8_t* buf = createReplytVersion();
            if(send(fd,buf,sizeof(*buf),0) == -1){
                perror("send error");
                //ignore
            }
            break;
        }
        case SectionStatus.REQUEST:{
            struct Request request = new Request();
            if(!decodeRequest(&request,buf)){
                perror("decode error");
                *(this->wapper_)->remove(fd);
                return;
            }
            if(request.CMD == '\x01'){
                uint8_t result = this->wapper_->createTcp(request.DST_ADDR,request.DST_PORT,
                    &(section->watch_fd),section));
                uint8_t* buf = createReplies(result)
                if(send(fd,buf,sizeof(buf),0) == -1){
                    perror("send error");
                    //double check
                    //*(this->wapper_)->remove(fd);
                    return;
                }
            }else if(request.CMD == '\x03'){
                // uint8_t result = this->wapper_->watchUdp(request.DST_ADDR,request.DST_PORT,
                //     &(section->watch_fd),section));

            }
            break;
        }
        case SectionStatus.REDY:{
            if(*section->tcp){
                if(*section.watch_fd < 0) throw "connection not ready!";
                //forward
                int to_fd = fd == *section.fd ? *section.watch_fd : *section.fd;
                struct LinkBuff* next = buf;
                while(next){
                    if(send(to_fd,next.buf,next.size,0) == -1){
                        perror("send error");
                    }
                    next = *next->next;
                }
            }else{
                //udp associate
                //TODO
            }
            break;
        }
        case SectionStatus.CLOSE:{
            this->onDestory(ptr,fd);
            break;
        }
        default:{
            break;
        }
    }
    //clean buffer
    struct LinkBuff* next = buf;
    struct LinkBuff* tmp;
    while(next){
        tmp = *next->next;
        delete *next;
        next = tmp;
    }
};

int main(int argc, const char * argv[]) {
    Socks5Server server();
    server.forever(1080);
}