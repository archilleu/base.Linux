#!/usr/bin/python3
# test target: test_datagram_socket.cc

import socket
import threading

HOST = "127.0.0.1"
PORT = 9999

def Test_Receiver():
    import pdb; pdb.set_trace()
    times = 1000;
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    for i in range(times):
        print('times:', i)
        s.sendto(b'mama', (HOST, PORT))

if '__main__' == __name__:
    print('test receiver')
    Test_Receiver()
