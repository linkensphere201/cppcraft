libdir := $(CURDIR)/../../staticlibs
libgtest := gtest

SPDLOG_DIR := $(libdir)/spdlog-1.8.2
SPDLOG_LIB := spdlog

lib_this_dir := $(CURDIR)/../libs
inc_this_dir := $(CURDIR)/../include

EXESRCS := $(shell ls *.cc)
EXES 		:= $(EXESRCS:.cc=.out)

CXX 			:= g++
CXXFLAGS 	:= -std=c++11 -O0 -ggdb \
	-I$(inc_this_dir)		\
	-I$(SPDLOG_DIR)/include 				\
	-I$(libdir)/$(libgtest)/include

LDFLAGS  	:= -L$(lib_this_dir)		\
	-L$(libdir)/$(libgtest)/lib64 	\
	-lcommon												\
	-lgtest 												\
	-lpthread

$(EXES): %.out:%.cc
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

.PHONY:
clean:
	rm -rf *.out
