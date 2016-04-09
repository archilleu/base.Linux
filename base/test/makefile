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
	_LIB_SHARE=-L ../.
	_LIB_SHARE_NAME=-lbase.linux
	_LIB_PUBLIC_NAME=-lpthread -lmysqlclient
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
objects=main.o test_base.o test_main.o test_memory_block.o test_timestamp.o test_function.o	\
		test_thread.o test_append_file.o test_log_file.o test_mysql_kit.o test_mysql_pool.o	\
		test_json.o test_md5.o test_semaphore.o test_computer_info.o
#---------------------------------------------------------------------------
main : $(objects)
	$(CXX_COMPILER) -o $(_OBJ_NAME) $(objects) $(_LIB_SHARE)  $(_LIB_SHARE_NAME) $(_LIB_PUBLIC_NAME);

main.o : main.cc
	$(CXX_COMPILER) $(CXX_FLAGS) $(_LIB_SHARE_HEADER) -c $(_SRC_PATH)/main.cc 

test_base.o : test_base.cc test_base.h
	$(CXX_COMPILER) $(CXX_FLAGS) $(_LIB_SHARE_HEADER) -c $(_SRC_PATH)/test_base.cc

test_main.o : test_main.cc test_main.h
	$(CXX_COMPILER) $(CXX_FLAGS) $(_LIB_SHARE_HEADER) -c $(_SRC_PATH)/test_main.cc

test_memory_block.o : test_memory_block.cc test_memory_block.h
	$(CXX_COMPILER) $(CXX_FLAGS) $(_LIB_SHARE_HEADER) -c $(_SRC_PATH)/test_memory_block.cc
	
test_timestamp.o : test_timestamp.cc test_timestamp.h
	$(CXX_COMPILER) $(CXX_FLAGS) $(_LIB_SHARE_HEADER) -c $(_SRC_PATH)/test_timestamp.cc
	
test_function.o : test_function.cc test_function.h
	$(CXX_COMPILER) $(CXX_FLAGS) $(_LIB_SHARE_HEADER) -c $(_SRC_PATH)/test_function.cc

test_thread.o : test_thread.cc test_thread.h
	$(CXX_COMPILER) $(CXX_FLAGS) $(_LIB_SHARE_HEADER) -c $(_SRC_PATH)/test_thread.cc

test_append_file.o : test_append_file.cc test_append_file.h
	$(CXX_COMPILER) $(CXX_FLAGS) $(_LIB_SHARE_HEADER) -c $(_SRC_PATH)/test_append_file.cc

test_log_file.o : test_log_file.cc test_log_file.h
	$(CXX_COMPILER) $(CXX_FLAGS) $(_LIB_SHARE_HEADER) -c $(_SRC_PATH)/test_log_file.cc

test_mysql_kit.o : test_mysql_kit.cc test_mysql_kit.h
	$(CXX_COMPILER) $(CXX_FLAGS) $(_LIB_SHARE_HEADER) -c $(_SRC_PATH)/test_mysql_kit.cc

test_mysql_pool.o : test_mysql_pool.cc test_mysql_pool.h
	$(CXX_COMPILER) $(CXX_FLAGS) $(_LIB_SHARE_HEADER) -c $(_SRC_PATH)/test_mysql_pool.cc

test_json.o : test_json.cc test_json.h
	$(CXX_COMPILER) $(CXX_FLAGS) $(_LIB_SHARE_HEADER) -c $(_SRC_PATH)/test_json.cc

test_md5.o : test_md5.cc test_md5.h
	$(CXX_COMPILER) $(CXX_FLAGS) $(_LIB_SHARE_HEADER) -c $(_SRC_PATH)/test_md5.cc

test_semaphore.o : test_semaphore.cc test_semaphore.h
	$(CXX_COMPILER) $(CXX_FLAGS) $(_LIB_SHARE_HEADER) -c $(_SRC_PATH)/test_semaphore.cc

test_computer_info.o : test_computer_info.cc test_computer_info.h
	$(CXX_COMPILER) $(CXX_FLAGS) $(_LIB_SHARE_HEADER) -c $(_SRC_PATH)/test_computer_info.cc

#---------------------------------------------------------------------------
clean:	
	-@ rm *.o $(_OBJ_NAME)
