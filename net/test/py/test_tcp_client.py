import socket
import select
import sys
import epoll_util

class EpollConnector:
    '''generic epoll connectors which connect to down stream server'''

    def __init__(self, num, srv):
        self.num = num
        self.srv_addr = srv
        #status
        self.status = epoll_util.begin_status
        # initial epoll fileno
        self.epoll = select.epoll()

    def set_epoll(self, poll):
        self.epoll.close()
        self.epoll = poll

    def init_epoll(self):
        # fileno set for connection
        self.conns_index = {}
        self.connections = {}

        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            s.connect(self.srv_addr)
            s.setblocking(0)
            fileno = s.fileno()
            self.epoll.register(fileno, select.EPOLLOUT | select.EPOLLIN | select.EPOLLET)
        except socket.error:
            print('ERROR in connect to ',  self.srv_addr)
            sys.exit(1)
        else:
            self.conns_index[fileno] = i
            self.connections[fileno] = s

    def hup_epoll(self, fileno):
        self.recover(fileno)
        self.epoll.unregister(fileno)
        self.connections[fileno].close()
        del self.connections[fileno]
        del self.requests[fileno]
        del self.responses[fileno]
        del self.conns_index[fileno]

    def close_epoll(self):
        self.epoll.close()

    def do_epoll(self, fileno, event):
        try:
            if event & select.EPOLLOUT:
                pass
            elif event & select.EPOLLIN:
                pass
            elif event & select.EPOLLHUP:
                pass
        except:
            raise

    def loop_epoll(self):
        try:
            while self.status != epoll_util.close_status:
                events = self.epoll.poll()
                for fileno, event in events:
                    self.do_epoll(fileno, event)
        finally:
            self.close_epoll()


if __name__ == "__main__":
    EpollConnector srv
    srv.init_epoll()
    srv.loop_epoll()
