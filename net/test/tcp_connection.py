#!/usr/bin/python3
# test target: test_tcp_connection

import socket
import threading

HOST = "127.0.0.1"
PORT = 9999

clinet_nums = 1
client_list = []
cond = threading.Condition()

def ClientConnect():
    for i in range(clinet_nums):
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM);
        client.connect((HOST, PORT))
        cond.acquire()
        client_list.append(client)
        cond.notify()
        print("connect nums:peer:", client.getpeername(), "local:", client.getsockname())
        cond.release()

        return client;

def ClientDisconnect():
    for i in range(clinet_nums):
        cond.acquire()
        while(len(client_list) == 0):
            cond.wait();
        #import pdb;pdb.set_trace();
        client = client_list.pop()
        print("disconnect num peer:", client.getpeername(), "local:", client.getsockname())
        client.close()

        cond.release()

def ClientSendData(conn):
    import pdb;pdb.set_trace()
    str_data    = "hello wo lai le!"
    byte_data   = str_data.encode("utf-8")
    byte_len    = len(byte_data)

    conn.send(byte_data);
    recv_data = conn.recv(byte_len)

    if str_data == recv_data.decode("utf-8"):
        print("right\n")
    else:
        print("error\n")

if "__main__" == __name__:
    #测试单线程
    client = ClientConnect()
    ClientSendData(client);

    ClientDisconnect();
    client_list = []

    #测试多线程
    t_connect = []
    t_disconnect = []
    for i in range(5):
        t_connect.append(threading.Thread(target=ClientConnect));
        t_disconnect.append(threading.Thread(target=ClientDisconnect));

    for i in range(5):
        t_connect[i].start();
        t_disconnect[i].start();

    for i in range(5):
        t_connect[i].join();
        t_disconnect[i].join();

    print("finished")


