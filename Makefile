CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
ASAN_FLAGS = -fsanitize=address,undefined -fno-omit-frame-pointer -g

# Automatically link system GoogleTest if available, else link pthread
HAS_GTEST := $(shell (which pkg-config >/dev/null 2>&1 && pkg-config --exists gtest 2>/dev/null && echo 1) || (test -f /usr/lib/x86_64-linux-gnu/libgtest.a && echo 1) || (test -f /usr/lib/libgtest.a && echo 1) || echo 0)
ifeq ($(HAS_GTEST), 1)
    TEST_LIBS = -lgtest -lgtest_main -pthread
else
    TEST_LIBS = -pthread
endif

TARGET = program.exe
TEST_TARGET = run_tests

SRCS = program.cpp User.cpp Account.cpp
TEST_SRCS = tests/test_finance.cpp User.cpp Account.cpp

.PHONY: all build test test-asan clean help

all: build

build: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)
	@echo "Build complete: $(TARGET)"

# Build and execute tests with AddressSanitizer
test: $(TEST_TARGET)
	@echo "Executing test suite..."
	./$(TEST_TARGET)

test-asan: test

$(TEST_TARGET): $(TEST_SRCS)
	$(CXX) -std=c++17 -Wall -Wextra $(ASAN_FLAGS) $(TEST_SRCS) $(TEST_LIBS) -o $(TEST_TARGET)

clean:
	rm -f $(TARGET) $(TEST_TARGET) *.o temp_test*.json

help:
	@echo "Available make targets:"
	@echo "  make build      - Compile the main application ($(TARGET))"
	@echo "  make test       - Build and run the test suite with ASan (-fsanitize=address,undefined)"
	@echo "  make clean      - Remove compiled binaries and test temporary files"
	@echo "  make help       - Display this help message"
