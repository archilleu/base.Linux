#!/usr/bin/python3
# test target: test_tcp_client

import socket
import inspect
import struct
import select
import sys
import random
import pdb
import socket
import threading

HOST = "127.0.0.1"
PORT = 9981

LEN = 1024

def SendAndRecv(conn_ptr):

    print("connet start")
    while True:
        if 0 == random.randint(0, 50):
            print("disconnect peer:", conn_ptr.getpeername(), "local:",conn_ptr.getsockname())
            conn_ptr.close();
            break;

        try:
            data = conn_ptr.recv(LEN, socket.MSG_WAITALL)
            if len(data) == LEN:
                conn_ptr.send(data)

        except OSError as e:
            break;

    return

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)
s.bind((HOST,PORT))
s.listen(5)

print("start svr")
while True:
    client = s.accept();
    print("connect peer:", client[0].getpeername(), "local:", client[0].getsockname())
    threading.Thread(target=SendAndRecv, args=(client[0],)).start()
