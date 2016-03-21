#---------------------------------------------------------------------------
#param debug=y表示编译debug版本
VPATH = base
#_OBJ_NAME			:目标名字
#_SRC_PATH			:源码路径
#_LIB_SHARE			:包含库
#_LIB_SHARE_HEADER	:包含库头文件
#CXX_COMPILER		:编译器
#CXX_FLAGS			:编译选项
#CXX_COMPILER_LIB	:编译指令
	_OBJ_NAME=libnet.linux.a
	_SRC_PATH=.
	_LIB_SHARE=""			
	_LIB_SHARE_HEADER=""
#	_LIB_SHARE_NAME=-lbase.linux该库依赖baselinux
	CXX_COMPILER_LIB=ar cvr -s
	CXX_COMPILER=g++

#---------------------------------------------------------------------------
	_CFLAGS=-D_REENTRANT -Wall -DCHECK_PTHREAD_RETURN_VALUE -D_FILE_OFFSET_BITS=64 -Wextra -Werror -Wconversion -Wno-unused-parameter -Wold-style-cast -Wpointer-arith -Wshadow -Wwrite-strings -std=c++11
ifeq ($(debug),y)
	CXX_FLAGS=$(_CFLAGS)
	CXX_FLAGS+=-g -D_DEBUG
else
	CXX_FLAGS=$(_CFLAGS)
	CXX_FLAGS+=-O2 -DNDEBUG
endif
#---------------------------------------------------------------------------
objects=event_loop.o net_log.o channel.o poller.o timer_task.o timer_task_queue.o event_loop_thread.o	\
		inet_address.o socket.o acceptor.o event_loop_thread_pool.o tcp_server.o buffer.o tcp_connection.o	\
		datagram_packet.o datagram_socket.o
#---------------------------------------------------------------------------
main : $(objects)
	$(CXX_COMPILER_LIB) $(_OBJ_NAME) $(objects)

event_loop.o : event_loop.h event_loop.cc
	$(CXX_COMPILER) $(CXX_FLAGS) -c event_loop.cc

net_log.o : net_log.h net_log.cc
	$(CXX_COMPILER) $(CXX_FLAGS) -c net_log.cc

channel.o : channel.h channel.cc
	$(CXX_COMPILER) $(CXX_FLAGS) -c channel.cc

poller.o : poller.h poller.cc
	$(CXX_COMPILER) $(CXX_FLAGS) -c poller.cc

timer_task.o : timer_task.h timer_task.cc
	$(CXX_COMPILER) $(CXX_FLAGS) -c timer_task.cc

timer_task_queue.o : timer_task_queue.h timer_task_queue.cc
	$(CXX_COMPILER) $(CXX_FLAGS) -c timer_task_queue.cc

event_loop_thread.o : event_loop_thread.h event_loop_thread.cc
	$(CXX_COMPILER) $(CXX_FLAGS) -c event_loop_thread.cc

inet_address.o : inet_address.h inet_address.cc
	$(CXX_COMPILER) $(CXX_FLAGS) -c inet_address.cc

socket.o : socket.h socket.cc
	$(CXX_COMPILER) $(CXX_FLAGS) -c socket.cc

acceptor.o : acceptor.h acceptor.cc
	$(CXX_COMPILER) $(CXX_FLAGS) -c acceptor.cc

event_loop_thread_pool.o : event_loop_thread_pool.h event_loop_thread_pool.cc
	$(CXX_COMPILER) $(CXX_FLAGS) -c event_loop_thread_pool.cc

tcp_server.o : tcp_server.h tcp_server.cc
	$(CXX_COMPILER) $(CXX_FLAGS) -c tcp_server.cc

tcp_connection.o : tcp_connection.h tcp_connection.cc
	$(CXX_COMPILER) $(CXX_FLAGS) -c tcp_connection.cc

buffer.o : buffer.h buffer.cc
	$(CXX_COMPILER) $(CXX_FLAGS) -c buffer.cc

datagram_packet.o : datagram_packet.h datagram_packet.cc
	$(CXX_COMPILER) $(CXX_FLAGS) -c datagram_packet.cc

datagram_socket.o : datagram_socket.h datagram_socket.cc
	$(CXX_COMPILER) $(CXX_FLAGS) -c datagram_socket.cc

#---------------------------------------------------------------------------
clean:	
	-@ rm *.o *.a
#---------------------------------------------------------------------------
