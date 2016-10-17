#---------------------------------------------------------------------------
#SOURCE 			目录下所有的C++文件
#OBJS				SOURCE对应的中间文件
#CXX				编译器
#CFLAGS 			编译选项
#LDFLAGS 			链接动态库
#CXX_COMPILER_LIB	编译指令
#TARGET_NAME		目标名字
#TARGET_HEADER		头文件
#---------------------------------------------------------------------------
SOURCE := $(wildcard *.cc)
OBJS := $(patsubst %.cc,%.o,$(SOURCE))
CXX=g++
LDFLAGS+=
CXX_COMPILER_LIB=ar cvr
TARGET_NAME=libnet.a
CXX_FLAGS+=-D_REENTRANT -Wall -DCHECK_PTHREAD_RETURN_VALUE -D_FILE_OFFSET_BITS=64 -Wextra -Werror -Wconversion -Wno-unused-parameter -Wold-style-cast -Wpointer-arith -Wshadow -Wwrite-strings -std=c++11
ifeq ($(debug),y)
	CXX_FLAGS+=-g -D_DEBUG
else
	CXX_FLAGS+=-O2 -DNDEBUG
endif
#---------------------------------------------------------------------------
all:$(TARGET_NAME)

%.o: %.cc
	$(CXX) $(CXX_FLAGS) -c $<

$(TARGET_NAME):$(OBJS)
	$(CXX_COMPILER_LIB) $(TARGET_NAME) $(OBJS)

.PHONY:clean
clean:
	rm -f *.o *.d $(TARGET_NAME)
	rm -rf ./bin

.PHONY:install
install:
	rm -rf ./bin
	@if [ ! -d "./bin" ]; then\
		mkdir bin;\
	fi

	cp buffer.h callback.h datagram_packet.h datagram_socket.h event_loop.h timer_task.h timer_task_id.h timer_task_queue.h inet_address.h socket.h tcp_client.h  tcp_server.h tcp_connection.h udp_server.h $(TARGET_NAME) ./bin

#---------------------------------------------------------------------------
