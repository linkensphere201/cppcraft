include ../../depends.mk
include ../../depends_tests.mk

CXX := c++

MYCXXFLAGS := -std=c++17 -ggdb \
		-I. \
		-I./include \
		-I../../../include \

MYLDF := -lfolly \
-lgtest	\
-lpthread \
-levent \
-liberty \
-ldouble-conversion \
-lunwind \
-lutils \
-lspdlog \
-lfizz -lgflags -lglog -lfmt -laio \
-lfolly \
-lboost_context -lboost_program_options -lboost_thread -lboost_filesystem -lboost_system -lboost_regex -lboost_chrono \
-llzma -lz -lsnappy -llz4 -lzstd -lbz2 \
-lsodium -lssl -lcrypto -lz -ldl

MYLDFLAGS := -L./lib -L./lib64 -L/lib64 -L../../libs \
             $(MYLDF)

$(TESTS_OBJS): %.o:%.cc
	$(CXX) -c $^ $(MYCXXFLAGS)

$(TESTS_TGTS): %.out:%.o
	$(CXX) -o $@ $^ $(MYLDFLAGS)
