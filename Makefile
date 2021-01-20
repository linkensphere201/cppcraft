basedir := $(CURDIR)/thirdparty/sources
builddir := $(CURDIR)/build
targetdir := $(CURDIR)/staticlibs

lib_curl := curl-7.74.0
src_curl := curl-7.74.0.tar.gz

lib_spdlog := spdlog-1.8.2
src_spdlog := spdlog-1.8.2.zip

lib_pb := protobuf-3.14.0
src_pb := protobuf-cpp-3.14.0.tar.gz
bin_pb := protoc-3.14.0-linux-x86_64.zip

lib_rocksdb := rocksdb-6.15.2
src_rocksdb := rocksdb-v6.15.2.tar.gz

lib_gtest := gtest

all: prepare curl spdlog protobuf rocksdb gtest
	@tree $(targetdir)

prepare:
	@mkdir -p $(builddir)
	@mkdir -p $(targetdir)

curl:
	@echo "build : $(lib_curl)"
	@echo "target: $(targetdir)/$(lib_curl)"
	mkdir -p $(targetdir)/$(lib_curl)
	tar zxvf $(basedir)/$(src_curl) -C $(builddir)
	cd $(builddir)/$(lib_curl); ./configure --prefix=$(targetdir)/$(lib_curl); 
	make -C $(builddir)/$(lib_curl) && make -C $(builddir)/$(lib_curl) install

spdlog:
	@echo "build : $(lib_spdlog)"
	@echo "target: $(targetdir)/$(lib_spdlog)"
	mkdir -p $(targetdir)/$(lib_spdlog)
	unzip $(basedir)/$(src_spdlog) -d $(builddir)
	cd $(builddir)/$(lib_spdlog) && mkdir build && cd build && cmake -DCMAKE_INSTALL_PREFIX=$(targetdir)/$(lib_spdlog) ..
	make -C $(builddir)/$(lib_spdlog)/build -j 4 && make -C $(builddir)/$(lib_spdlog)/build install

protobuf:
	@echo "build : $(lib_pb)"
	@echo "target: $(targetdir)/$(lib_pb)"
	mkdir -p $(targetdir)/$(lib_pb)
	tar zxvf $(basedir)/$(src_pb) -C $(builddir)
	cd $(builddir)/$(lib_pb); ./configure --prefix=$(targetdir)/$(lib_pb);
	make -C $(builddir)/$(lib_pb) -j 4 && make -C $(builddir)/$(lib_pb) install

rocksdb: 
	@echo "build : $(lib_rocksdb)"
	@echo "target: $(targetdir)/$(lib_rocksdb)"
	mkdir -p $(targetdir)/$(lib_rocksdb)/bin
	mkdir -p $(targetdir)/$(lib_rocksdb)/libs
	mkdir -p $(targetdir)/$(lib_rocksdb)/include
	tar zxvf $(basedir)/$(src_rocksdb) -C $(builddir)
	make -C $(builddir)/$(lib_rocksdb) ldb sst_dump
	make -C $(builddir)/$(lib_rocksdb) static_lib
	cp $(builddir)/$(lib_rocksdb)/librocksdb*.a $(targetdir)/$(lib_rocksdb)/libs
	cp $(builddir)/$(lib_rocksdb)/ldb $(targetdir)/$(lib_rocksdb)/bin
	cp $(builddir)/$(lib_rocksdb)/sst_dump $(targetdir)/$(lib_rocksdb)/bin
	cp $(builddir)/$(lib_rocksdb)/include $(targetdir)/$(lib_rocksdb)/ -r

gtest:
	@echo "build : $(lib_gtest)"
	@echo "target: $(targetdir)/$(lib_gtest)"
	mkdir -p $(builddir)/$(lib_gtest)
	mkdir -p $(targetdir)/$(lib_gtest)
	cd $(builddir)/$(lib_gtest); cmake $(basedir)/$(lib_gtest) -DCMAKE_INSTALL_PREFIX=$(targetdir)/$(lib_gtest) && make && make install

.PTHONY:
clean:
	rm -rf $(builddir)
	rm -rf $(targetdir)
