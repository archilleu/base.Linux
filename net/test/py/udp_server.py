#!/usr/bin/python3
# test target: test_datagram_socket.cc

import socket
import threading
import random

HOST = "127.0.0.1"
PORT = 9999

times = 0

def SendAndRecv():
    for i in range(1000):
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        byte_data   = bytearray(random.randint(1, 1500))
        byte_len    = len(byte_data)
        for i in range(byte_len):
            v = random.randint(0, 255);
            byte_data[i] = v;

        s.sendto(byte_data, (HOST, PORT))
        pair = s.recvfrom(1500)

        if byte_len != len(pair[0]):
            assert False, "len error"

        if byte_data != pair[0]:
            assert False, "dat error"

        global times
        times += 1
        print("times: ", times)

def Test_Receiver():

    #单线程
    SendAndRecv()

    #多线程
    thread_num = 500
    for i in range(thread_num):
        threading.Thread(target=SendAndRecv).start()

if '__main__' == __name__:
    print('test receiver')
    Test_Receiver()
