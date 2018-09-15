//
//  socks5server.cc
//
//  Created by 吴怡生 on 13/03/2018.
//  Copyright © 2018 yeshen.org. All rights reserved.
//

#include <stdlib.h>
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
#include "common.h"

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
    section->tcp = ((type & TYPE_TCP) != 0);
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

void Socks5Server::onWritable(void* ptr,int fd){
    if(!ptr || fd < 0)return;
    struct Section* section = (struct Section*)ptr;
    if(section->status==SS_REDY){
        if(section->tcp){
            int to_fd = fd == section->fd ? section->watch_fd : section->fd;
            struct LinkBuff* next = fd == section->fd ? section->insidebuff : section->outsidebuff;
            cout << "to:" << to_fd << ",from:" << fd;
            while(next){
                if(send(fd,next->buf,next->size,0) == -1){
                    if(errno==EAGAIN){
                        if(fd==section->fd){
                            section->insidebuff = next;
                        }else{
                            section->outsidebuff = next;
                        }
                        return;
                    }
                }else{
                    struct LinkBuff* tmp = next->next;
                    free(next->buf);
                    delete next;
                    next=tmp;
                }
            }
        }else{

        }
    }
}

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
                LinkBuffUtil::dofree(buf);
                return;
            }
            if(version->VER != ACCEPT_VERSION){
                this->wapper_->remove(fd);
                LinkBuffUtil::dofree(buf);
                return;
            }
            uint8_t* response = Socks5::createReplytVersion();
            if(send(fd,response,2,0) == -1){
                perror("send error");
                LinkBuffUtil::dofree(buf);
                return;
            }
            section->status = SS_REQUEST;
            LinkBuffUtil::dofree(buf);
            break;
        }
        case SS_REQUEST:{
            cout << "SS_REQUEST" << fd << endl;
            struct Request* request = new Request();
            if(!Socks5::decodeRequest(request,buf)){
                perror("decode error");
                this->wapper_->remove(fd);
                LinkBuffUtil::dofree(buf);
                return;
            }
            if(request->CMD == '\x01'){
                uint8_t result = this->wapper_->createTcp(
                    request->DST_ADDR,
                    request->DST_PORT,
                    &(section->watch_fd),
                    section);
                uint8_t* response = Socks5::createReplies(result);
                if(send(fd,response,10,0) == -1){
                    perror("send error");
                    //double check
                    //*(this->wapper_)->remove(fd);
                    LinkBuffUtil::dofree(buf);
                    return;
                }
                section->status = SS_REDY;
            }else if(request->CMD == '\x03'){
                // uint8_t result = this->wapper_->watchUdp ...

            }
            LinkBuffUtil::dofree(buf);
            break;
        }
        case SS_REDY:{
            cout << "SS_REDY" << fd << endl;
            if(section->tcp){
                if(section->watch_fd < 0) throw "connection not ready!";
                int to_fd = fd == section->fd ? section->watch_fd : section->fd;
                //forward
                struct LinkBuff* cache = fd==section->fd ? section->insidebuff : section->outsidebuff;
                struct LinkBuff* next;
                if(cache){
                    next = cache;
                    while(next->next){
                        next=next->next;
                    }
                    next->next=buf;
                    next = cache;
                }else{
                    next = buf;
                }
                cout << "try send:" << fd << ","<< next->size << endl;
                while(next){
                    if(send(to_fd,next->buf,next->size,0) == -1){
                        if(errno==EAGAIN){
                            if(fd==section->fd){
                                section->insidebuff = next;
                            }else{
                                section->outsidebuff = next;
                            }
                            return;
                        }else{
                            perror("send error");
                            return;
                        }
                    }else{
                        struct LinkBuff* tmp = next->next;
                        free(next->buf);
                        delete next;
                        next=tmp;
                    }
                }
            }else{
                cout << "udp: forwarding" << endl;
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
};

static volatile int keepRunning = 1;

void sig_handler(int sig){
    if(sig == SIGINT){
        keepRunning = 0;
    }
}

int main(int argc, const char * argv[]) {
    signal(SIGINT,sig_handler);

    Socks5Server* server=new Socks5Server();
    server->forever((uint16_t)1080);
}