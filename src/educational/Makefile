EXESRCS := $(shell ls *.cc)
EXES 		:= $(EXESRCS:.cc=.out)

include ../depends.mk

CXX 			:= g++
CXXFLAGS 	:= -std=c++11 -O0 -ggdb \
	-fno-elide-constructors \
	-I$(mylib_inc)		\
	-I$(gtest_inc) 		\
	-I$(spdlog_inc)

LDFLAGS  	:= -L$(mylib_dir)		\
	-L$(gtest_libdir) 	\
	-L$(spdlog_libdir)	\
	-lcommon						\
	-lgtest 						\
	-lpthread

$(EXES): %.out:%.cc
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

.PHONY:
clean:
	rm -rf *.out
