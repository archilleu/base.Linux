#!/usr/bin/python3
# test target: test_datagram_socket.cc

import socket
import threading

HOST = "127.0.0.1"
PORT = 11111
P = 9999

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind((HOST, PORT))
print("s bind")

while True:
    cli_dat = s.recvfrom(65536)
    print("rcv size:", len(cli_dat[0]), "rcv cli:", cli_dat[1])
    s.sendto(cli_dat[0], cli_dat[1])
