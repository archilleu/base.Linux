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
	_OBJ_NAME=libbase.Linux.a
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
objects=memory_block.o timestamp.o function.o thread.o append_file.o log_file.o
#---------------------------------------------------------------------------
main : $(objects)
	$(CXX_COMPILER_LIB) $(_OBJ_NAME) $(objects)

memory_block.o : memory_block.h memory_block.cc
	$(CXX_COMPILER) $(CXX_FLAGS) -c $(_SRC_PATH)/memory_block.cc

timestamp.o : timestamp.h timestamp.cc
	$(CXX_COMPILER) $(CXX_FLAGS) -c $(_SRC_PATH)/timestamp.cc 

function.o : function.h function.cc
	$(CXX_COMPILER) $(CXX_FLAGS) -c $(_SRC_PATH)/function.cc 

thread.o : thread.h thread.cc
	$(CXX_COMPILER) $(CXX_FLAGS) -c $(_SRC_PATH)/thread.cc 

append_file.o : append_file.h append_file.cc
	$(CXX_COMPILER) $(CXX_FLAGS) -c $(_SRC_PATH)/append_file.cc 

log_file.o : log_file.h log_file.cc
	$(CXX_COMPILER) $(CXX_FLAGS) -c $(_SRC_PATH)/log_file.cc 

#---------------------------------------------------------------------------
clean:	
	-@ rm *.o *.a
#---------------------------------------------------------------------------
