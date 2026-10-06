CXX      ?= g++
CXXFLAGS ?= -O2 -Wall -Wextra -pedantic
CXXFLAGS += -std=c++23

ifeq ($(OPENSSL_PREFIX),)
ifeq ($(shell uname -sm),Darwin arm64)
ifneq ($(wildcard /opt/homebrew/opt/openssl@3/lib),)
OPENSSL_PREFIX := /opt/homebrew/opt/openssl@3
endif
endif
endif

ifneq ($(OPENSSL_PREFIX),)
OPENSSL_CFLAGS := -I$(OPENSSL_PREFIX)/include
OPENSSL_LIBS   := -L$(OPENSSL_PREFIX)/lib -lcrypto
else
OPENSSL_CFLAGS := $(shell pkg-config --cflags libcrypto 2>/dev/null)
OPENSSL_LIBS   := $(shell pkg-config --libs libcrypto 2>/dev/null || echo -lcrypto)
endif

TARGET := assignment4
SRCS   := main.cpp
OBJS   := $(SRCS:.cpp=.o)
DEPS   := $(OBJS:.o=.d)

.PHONY: all run debug clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@ $(OPENSSL_LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(OPENSSL_CFLAGS) -MMD -MP -c $< -o $@

run: $(TARGET)
	./$(TARGET)

debug: CXXFLAGS := -std=c++23 -g -O0 -Wall -Wextra -pedantic -fsanitize=address,undefined
debug: LDFLAGS  += -fsanitize=address,undefined
debug: clean all

clean:
	rm -f $(TARGET) $(OBJS) $(DEPS)

-include $(DEPS)