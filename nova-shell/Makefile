# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Iinclude
TESTFLAGS := -g

# Directories
SRC_DIR := src
TEST_DIR := unit-tests
BUILD_DIR := build
TARGET := nova
TEST_BIN := tests

# Find all source files recursively
SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Exclude main.cpp from sources (for test build)
SRCS_NO_MAIN := $(filter-out $(SRC_DIR)/main.cpp, $(SRCS))
OBJS_NO_MAIN := $(SRCS_NO_MAIN:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Test sources
TEST_SRCS := $(shell find $(TEST_DIR) -name '*.cpp')
TEST_OBJS := $(TEST_SRCS:$(TEST_DIR)/%.cpp=$(BUILD_DIR)/$(TEST_DIR)/%.o)

# Default target
all: $(TARGET)

# Link main program
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(BUILD_DIR)/$@ $^

# Link test binary
$(TEST_BIN): $(OBJS_NO_MAIN) $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) $(TESTFLAGS) -o $(BUILD_DIR)/$@ $^

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile test files
$(BUILD_DIR)/$(TEST_DIR)/%.o: $(TEST_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Utility targets
clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(TEST_BIN)

run: all
	./$(BUILD_DIR)/$(TARGET)

test: $(TEST_BIN)
	./$(BUILD_DIR)/$(TEST_BIN)

.PHONY: all clean run test
