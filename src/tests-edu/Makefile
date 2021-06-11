EXESRCS := $(shell ls *.cc)
EXES 		:= $(EXESRCS:.cc=.out)

include ../depends.mk

CXX 			:= g++
CXXFLAGS 	:= -std=c++17 -O0 -ggdb \
	-fno-elide-constructors \
	-I$(mylib_inc)		\
	-I$(spdlog_inc)		\
	-I$(gtest_inc)    \
	-I$(fmt_inc)

LDFLAGS  	:= -L$(mylib_dir)		\
	-L$(gtest_libdir) 	\
	-L$(spdlog_libdir)	\
	-L$(fmt_libdir)			\
	-lutils							\
	-l$(gtest_lib) 			\
	-l$(spdlog_lib)			\
	-l$(fmt_lib)				\
	-lpthread

all: $(EXES)

$(EXES): %.out:%.cc
	$(CXX) -o $@ $^ $(CXXFLAGS) $(MYLDFLAGS)

.PHONY:
clean:
	rm -rf *.out
