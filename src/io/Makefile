include ../depends.mk
include ../depends_tests.mk

SRCS := $(shell ls *.cc)
SRCS := $(filter-out $(TESTS_SRCS), $(SRCS))
OBJS := $(SRCS:.cc=.o)

CXX       := g++
AR 				:= ar

CXXFLAGS  := -std=c++11 -ggdb 			\
	-I$(mylib_inc)										\
	-I$(spdlog_inc)

OUTPUT := "lib$(shell basename $(CURDIR)).a"

all: prepare $(OUTPUT)

prepare:
	@echo $(OUTPUT)
	mkdir -p $(mylib_dir)

$(OBJS): %.o:%.cc
	$(CXX) -o $@ -c $^ $(CXXFLAGS)

$(OUTPUT): $(OBJS)
	$(AR) cr $@ $^
	@cp $@ $(mylib_dir)

$(TESTS_OBJS): %.o:%.cc
	$(CXX) -c $^ $(MYCXXFLAGS)

$(TESTS_TGTS): %.out:%.o
	$(CXX) -o $@ $^ $(MYLDFLAGS)

clean:
	rm -f $(OUTPUT) $(TESTS_TGTS) *.o
