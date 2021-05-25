
#
# Note: this is a relative path for inner layer's Makefile.
#
DepRoot := $(CURDIR)/../../staticlibs

# my libs
mylib_dir := $(CURDIR)/../libs
mylib_inc := $(CURDIR)/../../include

# my libs


# 3rd party libs
spdlog_lib := spdlog
spdlog_dir := $(DepRoot)/spdlog-1.8.2
spdlog_inc := $(spdlog_dir)/include
spdlog_libdir := $(spdlog_dir)/lib64

rocksdb_lib := rocksdb
rocksdb_dir := $(DepRoot)/rocksdb-6.15.2
rocksdb_inc := $(rocksdb_dir)/include
rocksdb_libdir := $(rocksdb_dir)/libs

gtest_lib := gtest
gtest_dir := $(DepRoot)/gtest
gtest_inc := $(gtest_dir)/include
gtest_libdir := $(gtest_dir)/lib64

fmt_lib := fmt
fmt_dir := $(DepRoot)/fmt
fmt_inc := $(fmt_dir)/include
fmt_libdir := $(fmt_dir)/lib64

# not use for now
libhv_lib := hv
libhv_dir := $(DepRoot)/libhv
libhv_inc := $(libhv_dir)/include
libhv_libdir := $(libhv_dir)/lib

libevent_lib := event
libevent_dir := $(DepRoot)/libevent
libevent_inc := $(libevent_dir)/include
libevent_libdir := $(libevent_dir)/lib
libevent_static_files := $(shell ls $(libevent_libdir)/*.a)

libqueue_dir := $(DepRoot)/queues
libqueue_inc := $(libqueue_dir)/include

MYCXXFLAGS := -std=c++11 -O0 -ggdb  \
  -I$(mylib_inc)                  	\
  -I$(gtest_inc)                  	\
  -I$(spdlog_inc)                 	\
	-I$(fmt_inc)											\
  -I$(rocksdb_inc)									

MYLDFLAGS := $(libevent_static_files) \
	-L$(mylib_dir) \
  -L$(gtest_libdir)       	\
  -L$(spdlog_libdir)      	\
  -L$(rocksdb_libdir)     	\
	-L$(fmt_libdir)						\
  -lutils                 	\
  -lio                    	\
  -l$(gtest_lib)          	\
  -l$(spdlog_lib)         	\
  -l$(rocksdb_lib)        	\
	-l$(fmt_lib)							\
  -lpthread               	\
  -ldl
