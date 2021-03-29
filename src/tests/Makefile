DemoSources 		:= $(shell ls *.cc)
DemoObjects     := $(DemoSources:.cc=.o)
DemoExeutables 	:= $(DemoSources:.cc=.out)

include ../depends.mk

CXX := g++
CXXFLAGS := -std=c++11 -O0 -ggdb 	\
	-I$(mylib_inc) 									\
	-I$(gtest_inc) 									\
	-I$(spdlog_inc) 								\
	-I$(rocksdb_inc)

LDFLAGS := -L$(mylib_dir) \
	-L$(gtest_libdir) 			\
	-L$(spdlog_libdir) 			\
	-L$(rocksdb_libdir) 		\
	-lutils 								\
	-lio    								\
	-l$(gtest_lib) 					\
	-l$(spdlog_lib) 				\
	-l$(rocksdb_lib) 				\
	-lpthread								\
	-ldl

#
# at this demo, each `.out` only produced by its `.o` file.
#
$(DemoObjects): %.o:%.cc
	$(CXX) -c $^ $(CXXFLAGS)

$(DemoExeutables): %.out:%.o
	$(CXX) -o $@ $^ $(LDFLAGS)

.PHONY:
clean:
	rm -rf *.out *.o
