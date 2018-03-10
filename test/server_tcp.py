#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import socket
import datetime

def main():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    f=open('tcp','w')
    try:
        server_address = ('localhost', 10500)
        sock.bind(server_address)
        sock.listen(1)
        f.writelines('Watching %s:%s\n' % server_address)
        f.flush()
        while True:
            conn, client_address = sock.accept()
            try:
                f.writelines('%s from %s\n'% (datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S"),client_address))
                while True:
                    data = conn.recv(16)
                    if data:
                        conn.sendall(data);
                    else:
                        f.writelines('no more data from %s:%s\n' % client_address)
                        f.flush()
                        break;
            finally:
                conn.close()
    finally:
        f.close()
        sock.close()

if __name__ == '__main__':
    main()