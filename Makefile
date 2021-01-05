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


all: prepare curl spdlog protobuf
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


.PTHONY:
clean:
	rm -rf $(builddir)
	rm -rf $(targetdir)
