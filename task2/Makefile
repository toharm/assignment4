CXX      ?= g++
CXXFLAGS ?= -O2 -Wall -Wextra -pedantic
CXXFLAGS += -std=c++23 -pthread
LDFLAGS  += -pthread

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

TARGET := shadow_cracker

SRCS := $(filter-out main.cpp, $(wildcard *.cpp)) $(wildcard *.c)
OBJS := $(addsuffix .o,$(basename $(SRCS)))
DEPS := $(OBJS:.o=.d)

.PHONY: all run debug clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@ $(OPENSSL_LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(OPENSSL_CFLAGS) -MMD -MP -c $< -o $@

# Blowfish ships as C but must be built as C++ here (-x c++) so the mangled
# symbol names line up with bcrypt.o.
%.o: %.c
	$(CXX) $(CXXFLAGS) $(OPENSSL_CFLAGS) -x c++ -MMD -MP -c $< -o $@

run: $(TARGET)
	./$(TARGET)

SAN ?= address,undefined
debug: CXXFLAGS := -std=c++23 -pthread -g -O0 -Wall -Wextra -pedantic -fsanitize=$(SAN)
debug: LDFLAGS  += -fsanitize=$(SAN)
debug: clean all

clean:
	rm -f $(TARGET) $(OBJS) $(DEPS)

-include $(DEPS)