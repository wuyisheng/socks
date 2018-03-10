#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import socket
import datetime

def main():
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    f=open('udp','w')
    try:
        server_address = ('0.0.0.0', 10000)
        sock.bind(server_address)
        f.writelines('Watching %s:%s\n' % server_address)
        f.flush()

        while True:
            data, address = sock.recvfrom(4096)
            f.writelines(datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S"))
            f.writelines("-- %s>>%s\n" % (address,data))
            f.flush()
            if data:
                sock.sendto("echo", address)
                sock.sendto(data, address)
                sock.sendto("exit", address)
    finally:
        f.close();
        sock.close();

if __name__ == '__main__':
    main()