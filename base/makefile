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
	_OBJ_NAME=libbase.linux.a
	_SRC_PATH=./base
	_LIB_SHARE=""			
	_LIB_SHARE_HEADER=""	
	CXX_COMPILER_LIB=ar cvr
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
objects=memory_block.o timestamp.o function.o thread.o append_file.o log_file.o mysql_kit.o auto_transaction.o mysql_pool.o
#---------------------------------------------------------------------------
main : $(objects)
	$(CXX_COMPILER_LIB) $(_OBJ_NAME) $(objects)

memory_block.o : memory_block.h memory_block.cc
	$(CXX_COMPILER) $(CXX_FLAGS) -c memory_block.cc

timestamp.o : timestamp.h timestamp.cc
	$(CXX_COMPILER) $(CXX_FLAGS) -c timestamp.cc 

function.o : function.h function.cc
	$(CXX_COMPILER) $(CXX_FLAGS) -c function.cc 

thread.o : thread.h thread.cc
	$(CXX_COMPILER) $(CXX_FLAGS) -c thread.cc 

append_file.o : append_file.h append_file.cc
	$(CXX_COMPILER) $(CXX_FLAGS) -c append_file.cc 

log_file.o : log_file.h log_file.cc
	$(CXX_COMPILER) $(CXX_FLAGS) -c log_file.cc 

mysql_kit.o : mysql_kit.h mysql_kit.cc
	$(CXX_COMPILER) $(CXX_FLAGS) -c mysql_kit.cc 

auto_tansaction.o : auto_tansaction.h auto_tansaction.cc
	$(CXX_COMPILER) $(CXX_FLAGS) -c auto_tansaction.cc 

mysql_pool.o : mysql_pool.h mysql_pool.cc
	$(CXX_COMPILER) $(CXX_FLAGS) -c mysql_pool.cc 

#---------------------------------------------------------------------------
clean:	
	-@ rm *.o *.a
#---------------------------------------------------------------------------
