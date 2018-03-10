#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import socket
import datetime

SERVER_IP = "118.89.53.151"

def main():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_address = (SERVER_IP, 10500)
    print 'connecting to %s port %s' % server_address
    sock.connect(server_address)
    try:        
        message = 'This is the message.  It will be repeated.'
        print 'sending "%s"' % message
        sock.sendall(message)
        amount_received = 0
        amount_expected = len(message)
        while amount_received < amount_expected:
            data = sock.recv(16)
            amount_received += len(data)
            print 'received "%s"' % data
    finally:
        print 'closing socket'
        sock.close()

if __name__ == '__main__':
    main()