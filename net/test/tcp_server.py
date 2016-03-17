#!/usr/bin/python3
# test target: test_tcp_server

import socket

HOST = "127.0.0.1"
PORT = 9999
import pdb;pdb.set_trace()
client = socket.socket(socket.AF_INET, socket.SOCK_STREAM);
client.connect((HOST, PORT))
print("peer:", client.getpeername(), "local:", client.getsockname())
client.close()




