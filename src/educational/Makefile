libdir := $(CURDIR)/../../staticlibs
libgtest := gtest

EXESRCS := $(shell ls *.cc)
EXES 		:= $(EXESRCS:.cc=.out)

CXX 			:= g++
CXXFLAGS 	:= -I$(libdir)/$(libgtest)/include
LDFLAGS  	:= -L$(libdir)/$(libgtest)/lib64 	\
	-lgtest 																	\
	-lpthread

$(EXES): %.out:%.cc
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

.PHONY:
clean:
	rm -rf *.out
