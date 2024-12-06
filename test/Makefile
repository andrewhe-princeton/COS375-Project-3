# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

# Directories
SRC_DIR = ../src
BUILD_DIR = bin
OBJ_DIR = $(BUILD_DIR)/obj

# Define files to exclude
EXCLUDE_FILES = ../src/sim_cycle.cpp ../src/sim_funct.cpp ../src/cycle.cpp ../src/test_memory.cpp

# Source files and object files
SRC_FILES = $(filter-out $(EXCLUDE_FILES), $(wildcard $(SRC_DIR)/*.cpp))
OBJ_FILES = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir $(SRC_FILES)))

# Default target: Build only object files
all: $(OBJ_FILES)

# Rule to build object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build directory
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
