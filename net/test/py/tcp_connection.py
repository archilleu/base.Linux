#!/usr/bin/python3
# test target: test_tcp_connection

import socket
import threading
import random
import struct

HOST = "0.0.0.0"
PORT = 9999

REPLY = 1
NOTIFY = 2

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

def SendEncode(dat, conn_ptr):
        codec = struct.Struct(">ii")
        header = codec.pack(len(dat), REPLY)
        conn_ptr.send(header)
        conn_ptr.send(dat)

        return;

def RecvDecode(conn_ptr):
        rcv = conn_ptr.recv(8)
        if 0 == len(rcv):
            return None

        codec   = struct.Struct(">ii")
        header  = codec.unpack(rcv)

        print("type:", header[1], "size:", header[0])
        dat = conn_ptr.recv(header[0])
        return (dat, header[1])

def RecvNotify(dat, conn_ptr):
    print("recv notify")

    addr = conn_ptr.getsockname()
    str_addr = addr[0] + ":" + str(addr[1])

    if str_addr != dat.decode("utf-8"):
        assert False, "notify error"

def Reconnect(tname, conn_ptr):
    conn_ptr.close()
    conn_ptr = ClientConnect()
    client_list[tname] = conn_ptr
    return conn_ptr

client_list = []
def ClientSendData(tname, conn_ptr):
    for i in range(10000):
        print("thread name:", tname, "times:", i)
        byte_data   = bytearray(random.randint(1, 1024*64))
        byte_len    = len(byte_data)
        for i in range(byte_len):
            v = random.randint(0, 255)
            byte_data[i] = v;

        #import pdb; pdb.set_trace();
        try:
            SendEncode(byte_data, conn_ptr)
        except BrokenPipeError as e:
            print(e)
            conn_ptr = Reconnect(tname, conn_ptr)
            continue

        pair = RecvDecode(conn_ptr)

        if None == pair:
            conn_ptr = Reconnect(tname, conn_ptr)
            continue

        if NOTIFY == pair[1]:
            RecvNotify(pair[0], conn_ptr)
            pair = RecvDecode(conn_ptr)
            if None == pair:
                conn_ptr = Reconnect(tname, conn_ptr)
                continue

        if REPLY == pair[1]:
            if byte_data != pair[0]:
                import pdb; pdb.set_trace();
                assert False, "error"

        #随机断线~
        if 1 == random.randint(1, 50):
            conn_ptr = Reconnect(tname, conn_ptr)

    return

if "__main__" == __name__:
    #测试单线程
    #client = ClientConnect()
    #ClientSendData("s", client);
    #ClientDisconnect();

    #测试多线程
    client_nums = 1
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


