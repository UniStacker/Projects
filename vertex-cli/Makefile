# Directories
SRC_DIR := srcs
OUT_DIR := modules
BIN_DIR := bin
TARGET := vxc

# Compiler & Flags
CXX := g++
CXXFLAGS := -Wall -Wextra -O2
LDFLAGS := -lcrypto -lssl

# Automatically set parallel jobs to the number of CPU cores
MAKEFLAGS += -j$(shell nproc)

# Find all C++ source files
SRC_FILES := $(shell find $(SRC_DIR)/modules -type f -name "*.cpp")
BIN_FILES := $(patsubst $(SRC_DIR)/modules/%.cpp, $(OUT_DIR)/%, $(SRC_FILES))

# Default target: Build all modules and the main binary
all: $(BIN_FILES) $(BIN_DIR)/$(TARGET)

# Rule to compile modules
$(OUT_DIR)/%: $(SRC_DIR)/modules/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

# Rule to compile main binary
$(BIN_DIR)/$(TARGET): $(SRC_DIR)/$(TARGET).cpp
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

# Clean build files
clean:
	rm -rf $(OUT_DIR) $(BIN_DIR)

# Rebuild everything
rebuild: clean all

# Ensure these are treated as targets, not filenames
.PHONY: all clean rebuild
