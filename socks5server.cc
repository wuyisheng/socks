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
#include <cerrno>
#include <sys/types.h>
#include <stdio.h>
#include "socks5server.h"
#include "epollwapper.h"
#include "socks5.h"

using namespace std;

Socks5Server::Socks5Server(){
    this->wapper_ = new EpollWapper();
};

void Socks5Server::forever(uint16_t port){
    cout << "start" << endl;
    int fd;
    this->wapper_->create(this);
    this->wapper_->watchTcp(port,&fd,NULL);
    this->wapper_->wait();
};

void Socks5Server::onAccept(int fd,int type,void* data){
    struct Section* section =  new Section();
    section->fd = fd;
    section->tcp = type & TYPE_TCP != 0;
    section->status = SS_INIT;
    section->watch_fd = -1;
    ((struct EpollData*)data)->ptr = section;
}

void Socks5Server::onDestory(void* ptr,int fd){
    if(!ptr || fd < 0)return;
    this->wapper_->remove(fd);
    struct Section* section = (struct Section*)ptr;
    if(section->watch_fd > 0){
        this->wapper_->remove(section->watch_fd);
    }
    struct UDPPair* forwardlist = section->udp;
    while(forwardlist){
        this->wapper_->remove(forwardlist->in_fd);
        this->wapper_->remove(forwardlist->out_fd);
        forwardlist = forwardlist->next;
    }
    delete section;
};

void Socks5Server::onData(struct LinkBuff* buf,void* ptr,int fd){
    cout << "onData:" << fd << ptr << endl;
    if(!ptr || !buf ) return;
    struct Section* section = (struct Section*)ptr;
    switch(section->status){
        case SS_INIT:{
            cout << "SS_INIT" << fd << endl;
            struct RequestVersion* version = new RequestVersion();
            if(!Socks5::decodeRequestVersion(version,buf)){
                perror("decode error");
                this->wapper_->remove(fd);
                return;
            }
            if(version->VER != ACCEPT_VERSION){
                this->wapper_->remove(fd);
                return;
            }
            uint8_t* response = Socks5::createReplytVersion();
            
            //TODO here has a bug in response's size

            cout << "response,size:" << sizeof(*response) << endl;
            if(send(fd,response,sizeof(response),0) == -1){
                perror("send error");
                return;
            }
            section->status = SS_REQUEST;
            break;
        }
        case SS_REQUEST:{
            cout << "SS_REQUEST" << fd << endl;
            struct Request* request = new Request();
            if(!Socks5::decodeRequest(request,buf)){
                perror("decode error");
                this->wapper_->remove(fd);
                return;
            }
            if(request->CMD == '\x01'){
                uint8_t result = this->wapper_->createTcp(
                    request->DST_ADDR,
                    request->DST_PORT,
                    &(section->watch_fd),
                    section);
                uint8_t* response = Socks5::createReplies(result);
                if(send(fd,response,sizeof(response),0) == -1){
                    perror("send error");
                    //double check
                    //*(this->wapper_)->remove(fd);
                    return;
                }
                section->status = SS_REDY;
            }else if(request->CMD == '\x03'){
                // uint8_t result = this->wapper_->watchUdp ...

            }
            break;
        }
        case SS_REDY:{
             cout << "SS_REDY" << fd << endl;
            if(section->tcp){
                if(section->watch_fd < 0) throw "connection not ready!";
                //forward
                int to_fd = fd == section->fd ? section->watch_fd : section->fd;
                struct LinkBuff* next = buf;
                while(next){
                    if(send(to_fd,next->buf,next->size,0) == -1){
                        perror("send error");
                    }
                    next = next->next;
                }
            }else{
                //udp associate
                //TODO
            }
            break;
        }
        case SS_CLOSE:{
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
        tmp = next->next;
        delete next;
        next = tmp;
    }
};

int main(int argc, const char * argv[]) {
    Socks5Server* server=new Socks5Server();
    server->forever((uint16_t)1080);
}