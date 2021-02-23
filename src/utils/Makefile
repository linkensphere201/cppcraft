include ../depends.mk

SRCS := $(shell ls *.cc)
OBJS := $(SRCS:.cc=.o)

CXX       := g++
AR 				:= ar

CXXFLAGS  := -std=c++11 -ggdb 			\
	-I$(mylib_inc)										\
	-I$(spdlog_inc)

OUTPUT := libutils.a

all: prepare $(OUTPUT)

prepare:
	mkdir -p $(mylib_dir)

$(OBJS): %.o:%.cc
	$(CXX) -o $@ -c $^ $(CXXFLAGS)

$(OUTPUT): $(OBJS)
	$(AR) cr $@ $^
	@cp $@ $(mylib_dir)
