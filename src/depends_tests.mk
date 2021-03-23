TESTS_SRCS := $(shell ls test_*.cc)
TESTS_OBJS := $(TESTS_SRCS:.cc=.o)
TESTS_TGTS := $(TESTS_SRCS:.cc=.out)
