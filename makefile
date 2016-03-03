#---------------------------------------------------------------------------
#param debug=y表示编译debug版本
VPATH = base
#_OBJ_NAME			:目标名字
#_OBJ_PATH			:目标路径
#_SRC_PATH			:源码路径
#_LIB_SHARE			:包含库
#_LIB_SHARE_HEADER	:包含库头文件
#CXX_COMPILER		:编译器
#CXX_FLAGS			:编译选项
	CXX_COMPILER=g++
	CXX_COMPILER_LIB=ar cvr
	_OBJ_NAME=libbase.Linux.a
	_SRC_PATH=./base
	_LIB_SHARE=""			
	_LIB_SHARE_HEADER=""	

	_CFLAGS=-D_REENTRANT -Wall -DCHECK_PTHREAD_RETURN_VALUE -D_FILE_OFFSET_BITS=64 -Wextra -Werror -Wconversion -Wno-unused-parameter -Wold-style-cast -Wpointer-arith -Wshadow -Wwrite-strings -std=c++11
ifeq ($(debug),y)
	_OBJ_PATH=./base/debug
	CXX_FLAGS=$(_CFLAGS)
	CXX_FLAGS+=-g -D_DEBUG
else
	_OBJ_PATH=./base/release
	CXX_FLAGS=$(_CFLAGS)
	CXX_FLAGS+=-O2 -DNDEBUG
endif

objects=memory_block.o
main : $(objects)
	$(CXX_COMPILER_LIB) $(_OBJ_PATH)/$(_OBJ_NAME) $(_OBJ_PATH)/$(objects)

memory_block.o : memory_block.h memory_block.cc
	$(CXX_COMPILER) $(CXX_FLAGS) -c $(_SRC_PATH)/memory_block.cc -o $(_OBJ_PATH)/memory_block.o

clean:	
	-@ rm $(_OBJ_PATH)/*
