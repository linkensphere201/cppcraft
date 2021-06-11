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
	-L$(rocksdb_libdir)	\
	-lutils 								\
	-lio    								\
	-lzstd -lbz2 -llz4 -lz \
	-l$(gtest_lib) 					\
	-l$(spdlog_lib) 				\
	-l$(rocksdb_lib)	\
	-lpthread								\
	-ldl	\
	-static-libstdc++


#
# at this demo, each `.out` only produced by its `.o` file.
#
$(DemoObjects): %.o:%.cc
	$(CXX) -c $^ $(CXXFLAGS)

$(DemoExeutables): %.out:%.o
	$(CXX) -o $@ $^ $(LDFLAGS)
		#/opt/vesoft/third-party/lib/librocksdb.a \
		#/home/weijian.yang/WORKSPACE/install/lib64/libzstd.a \
		#/opt/vesoft/third-party/lib/libbz2.a \
		#/opt/vesoft/third-party/lib/liblz4.a \
		#/opt/vesoft/third-party/lib/libz.a \
		#/opt/vesoft/third-party/lib/liblz4.a \

.PHONY:
clean:
	rm -rf *.out *.o
