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
TARGET_NAME=TestNet
INCLUDE_PATH=-I ../bin
SHARE_LIP_PATH=-L ../bin
SHARE_LIB=-lnet -lbase
PUBLIC_SHARE_LIB=-lpthread -lmysqlclient
CXX_FLAGS+=-D_REENTRANT -Wall -DCHECK_PTHREAD_RETURN_VALUE -D_FILE_OFFSET_BITS=64 -Wextra -Werror -Wconversion -Wno-unused-parameter -Wold-style-cast -Wpointer-arith -Wshadow -Wwrite-strings -std=c++11
ifeq ($(debug),y)
	CXX_FLAGS+=-g -D_DEBUG
else
	CXX_FLAGS+=-O2 -DNDEBUG
endif
#---------------------------------------------------------------------------
all:$(TARGET_NAME)

%.o: %.cc
	$(CXX) $(CXX_FLAGS) $(INCLUDE_PATH) -c $<

$(TARGET_NAME):$(OBJS)
	$(CXX) $(OBJS) -o $(TARGET_NAME) $(SHARE_LIP_PATH) $(SHARE_LIB) $(PUBLIC_SHARE_LIB)

.PHONY:clean
clean:
	rm -f *.o *.d $(TARGET_NAME)

#---------------------------------------------------------------------------
