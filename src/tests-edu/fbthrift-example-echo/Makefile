SRCS := $(shell ls echo/gen-cpp2/*.cpp ./*.cpp)
OBJS := $(SRCS:.cpp=.o)
CXX       := /usr/bin/c++
AR        := ar

MYCXXFLAGS  := -std=c++17 -ggdb \
  -Iecho/gen-cpp2 \
  -I. \
	-I./include \
	-frtti

MYLDF := -lwangle \
-lfolly \
-lasync -lthriftprotocol -ltransport -lconcurrency -lthriftfrozen2 -lthrift-core -lrpcmetadata -lthriftmetadata \
-lfolly \
-lthriftcpp2 \
-lconcurrency -lasync -lthriftprotocol -ltransport \
-lwangle \
-lfolly \
-lfizz -lgflags -lglog -lfmt -laio \
-lboost_context -lboost_program_options -lboost_thread -lboost_filesystem -lboost_system -lboost_regex -lboost_chrono \
-llzma -lz -lsnappy -llz4 -lzstd -lbz2 \
-lpthread \
-levent	\
-liberty \
-ldouble-conversion \
-lunwind \
-lsnappy \
-lsodium -lssl -lcrypto -lz -ldl

A := ./lib64/libsnappy.a \
./lib/libfolly.a \
./lib/libfollybenchmark.a \
./lib/libfolly_exception_counter.a \
./lib/libfolly_exception_tracer.a \
./lib/libfolly_exception_tracer_base.a \
./lib/libboost_context.a \
./lib/libasync.a \
./lib/libconcurrency.a \
./lib/libmustache_lib.a \
./lib/librpcmetadata.a \
./lib/libthrift-core.a \
./lib/libthriftcpp2.a \
./lib/libthriftfrozen2.a \
./lib/libthriftmetadata.a \
./lib/libthriftprotocol.a \
./lib/libtransport.a \
./lib64/libsnappy.a \
./lib/libproxygenhttpserver.a \
./lib/libproxygen.a 

MYLDFLAGS := -L./lib -L./lib64 -L/lib64 \
						 $(MYLDF)

LIB := "lib$(shell basename $(CURDIR)).a"

all: echo server
prepare:
	@echo "SRCS:" $(SRCS)
	@echo "OBJS:" $(OBJS)
	@echo "MYCXXFLAGS:" $(MYCXXFLAGS)

$(OBJS): %.o:%.cpp
	$(CXX) -o $@ -c $^ $(MYCXXFLAGS)

$(LIB): $(OBJS)
	$(AR) cr $@ $^

# generate thrift objects
echo: echo.thrift
	mkdir -p $@
	thrift1 -v -r --gen mstch_cpp2 -o $@ $^

server: src/server_main.cpp $(LIB)
	$(CXX) -o $@ $^ $(A) $(MYCXXFLAGS) $(MYLDFLAGS)

client: src/client_main.cpp $(LIB)
	$(CXX) -o $@ $^ $(A) $(MYCXXFLAGS) $(MYLDFLAGS)
