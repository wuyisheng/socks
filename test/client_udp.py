#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import socket

SERVER_IP = "118.89.53.151"

def main():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    server_address = (SERVER_IP, 10000)
    message = 'hi,yeshen!'

    try:
        count = sock.sendto(message, server_address)
        if count > 0:
            print 'done send %s' % message
        while True:
            data, _ = sock.recvfrom(4096)
            print 'received "%s"' % data
            if data == "exit":
                break;
    finally:
        print 'closing socket'
        sock.close()

if __name__ == '__main__':
    main()