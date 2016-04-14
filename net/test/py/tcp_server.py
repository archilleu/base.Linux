#!/usr/bin/python3
# test target: test_tcp_client

import socket
import threading

HOST = "127.0.0.1"
PORT = 9981

def SendAndRecv(conn):

    while True:
        try:
            import pdb; pdb.set_trace()
            data = conn.recv(1024)
            if data:
                conn.send(data)
        except OSError as e:
            print(e)
            break;

    return

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)
s.bind((HOST,PORT))
s.listen(5)

print("start svr")
while True:
    client = s.accept();
    print("accept client:", client[1])
    threading.Thread(target=SendAndRecv)
