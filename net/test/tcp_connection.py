#!/usr/bin/python3
# test target: test_tcp_connection

import socket
import threading
import random

HOST = "127.0.0.1"
PORT = 9999

clinet_nums = 1
client_list = []
cond = threading.Condition()

def ClientConnect():
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM);
        client.connect((HOST, PORT))
        print("connect nums:peer:", client.getpeername(), "local:", client.getsockname())

        return client

def ClientDisconnect():
        print("disconnect num peer:", client.getpeername(), "local:", client.getsockname())
        client.close()

        return

client_list = []
def ClientSendData(tname, conn):
    for i in range(10000):
        print("thread name:", tname, "times:", i)

        byte_data   = bytearray(random.randint(1, 1024*64))
        byte_len    = len(byte_data)
        for i in range(byte_len):
            v = random.randint(0, 255);
            byte_data[i] = v;

        conn.send(byte_data);
        recv_data = conn.recv(byte_len)

        if byte_data != recv_data:
            assert false, "error"

        #随机断线~
        if 1 == random.randint(1, 50):
            if("s" == tname):
                continue

            conn.close()
            conn = ClientConnect()
            client_list[tname] = conn

    return

if "__main__" == __name__:
    #测试单线程
    #client = ClientConnect()
    #ClientSendData("s", client);
    #ClientDisconnect();

    #测试多线程
    client_nums = 50
    for i in range(client_nums):
        client_list.append(ClientConnect())

    t_connect = []
    for i in range(client_nums):
        t_connect.append(threading.Thread(target=ClientSendData,
                                          args=(i, client_list[i],)))

    for i in range(client_nums):
        t_connect[i].start()

    for i in range(client_nums):
        t_connect[i].join()

    print("finished")


