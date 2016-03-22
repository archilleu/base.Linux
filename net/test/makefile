#---------------------------------------------------------------------------
#param debug=y表示编译debug版本
VPATH = base
#_OBJ_NAME			:目标名字
#_SRC_PATH			:源码路径
#_LIB_SHARE			:包含库
#_LIB_SHARE_HEADER	:包含库头文件
#CXX_COMPILER		:编译器
#CXX_FLAGS			:编译选项
	_OBJ_NAME=test
	_SRC_PATH=.
	_LIB_SHARE=-L ../. -L ../../base/.
	_LIB_SHARE_INCLUDE=-I ../. -I ../../base/.
	_LIB_SHARE_NAME=-lnet.linux -lbase.linux
	_LIB_PUBLIC_NAME=-pthread
	CXX_COMPILER=g++

	_CFLAGS=-D_REENTRANT -Wall -DCHECK_PTHREAD_RETURN_VALUE -D_FILE_OFFSET_BITS=64 -Wextra -Werror -Wconversion -Wno-unused-parameter -Wold-style-cast -Wpointer-arith -Wshadow -Wwrite-strings -std=c++11
ifeq ($(debug),y)
	CXX_FLAGS=$(_CFLAGS)
	CXX_FLAGS+=-g -D_DEBUG
else
	CXX_FLAGS=$(_CFLAGS)
	CXX_FLAGS+=-O2 -DNDEBUG
endif
#---------------------------------------------------------------------------
objects=main.o test_base.o test_main.o test_event_loop.o test_event_loop_thread.o test_inet_address.o	\
		test_socket.o test_acceptor.o test_buffer.o test_tcp_server.o test_tcp_connection.o test_datagram_packet.o	\
		test_datagram_socket.o
#---------------------------------------------------------------------------
main : $(objects)
	$(CXX_COMPILER) -o $(_OBJ_NAME) $(objects) $(_LIB_SHARE) $(_LIB_SHARE_INCLUDE)  $(_LIB_SHARE_NAME) $(_LIB_PUBLIC_NAME);

main.o : main.cc
	$(CXX_COMPILER) $(CXX_FLAGS) $(_LIB_SHARE_HEADER) -c $(_SRC_PATH)/main.cc 

test_base.o : test_base.cc test_base.h
	$(CXX_COMPILER) $(CXX_FLAGS) $(_LIB_SHARE_HEADER) -c $(_SRC_PATH)/test_base.cc

test_main.o : test_main.cc test_main.h
	$(CXX_COMPILER) $(CXX_FLAGS) $(_LIB_SHARE_HEADER) -c $(_SRC_PATH)/test_main.cc

test_event_loop.o : test_event_loop.cc test_event_loop.h
	$(CXX_COMPILER) $(CXX_FLAGS) $(_LIB_SHARE_HEADER) -c $(_SRC_PATH)/test_event_loop.cc


test_event_loop_thread.o : test_event_loop_thread.cc test_event_loop_thread.h
	$(CXX_COMPILER) $(CXX_FLAGS) $(_LIB_SHARE_HEADER) -c $(_SRC_PATH)/test_event_loop_thread.cc

test_inet_address.o : test_inet_address.cc test_inet_address.h
	$(CXX_COMPILER) $(CXX_FLAGS) $(_LIB_SHARE_HEADER) -c $(_SRC_PATH)/test_inet_address.cc

test_socket.o : test_socket.cc test_socket.h
	$(CXX_COMPILER) $(CXX_FLAGS) $(_LIB_SHARE_HEADER) -c $(_SRC_PATH)/test_socket.cc

test_acceptor.o : test_acceptor.cc test_acceptor.h
	$(CXX_COMPILER) $(CXX_FLAGS) $(_LIB_SHARE_HEADER) -c $(_SRC_PATH)/test_acceptor.cc

test_buffer.o : test_buffer.cc test_buffer.h
	$(CXX_COMPILER) $(CXX_FLAGS) $(_LIB_SHARE_HEADER) -c $(_SRC_PATH)/test_buffer.cc

test_tcp_server.o : test_tcp_server.cc test_tcp_server.h
	$(CXX_COMPILER) $(CXX_FLAGS) $(_LIB_SHARE_HEADER) -c $(_SRC_PATH)/test_tcp_server.cc

test_tcp_connection.o : test_tcp_connection.cc test_tcp_connection.h
	$(CXX_COMPILER) $(CXX_FLAGS) $(_LIB_SHARE_HEADER) -c $(_SRC_PATH)/test_tcp_connection.cc

test_datagram_packet.o : test_datagram_packet.cc test_datagram_packet.h
	$(CXX_COMPILER) $(CXX_FLAGS) $(_LIB_SHARE_HEADER) -c $(_SRC_PATH)/test_datagram_packet.cc

test_datagram_socket.o : test_datagram_socket.cc test_datagram_socket.h
	$(CXX_COMPILER) $(CXX_FLAGS) $(_LIB_SHARE_HEADER) -c $(_SRC_PATH)/test_datagram_socket.cc

#---------------------------------------------------------------------------
clean:	
	-@ rm *.o $(_OBJ_NAME)
