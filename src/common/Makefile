libdir := $(CURDIR)/../../staticlibs

SRCS := $(shell ls *.cc)
OBJS := $(SRCS:.cc=.o)

INCLUDE := $(CURDIR)/../include
target  := $(CURDIR)/../libs

SPDLOG_DIR := $(libdir)/spdlog-1.8.2
SPDLOG_LIB := spdlog

CXX       := g++
AR 				:= ar

CXXFLAGS  := -std=c++11 -ggdb 			\
	-I$(INCLUDE)											\
	-I$(SPDLOG_DIR)/include

LDFLAGS   := -L$(SPDLOG_DIR)/lib64  \
	-lspdlog 													\
  -lpthread

prepare:
	mkdir -p $(target)

$(OBJS): %.o:%.cc
	$(CXX) -o $@ -c $^ $(CXXFLAGS)

libcommon: $(OBJS)
	$(AR) cr $@.a $^
	@cp $@.a $(target)
