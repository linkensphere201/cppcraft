
#
# Note: this is a relative path for inner layer's Makefile.
#
DepRoot := $(CURDIR)/../../staticlibs

# my libs
mylib_dir := $(CURDIR)/../libs
mylib_inc := $(CURDIR)/../include

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

