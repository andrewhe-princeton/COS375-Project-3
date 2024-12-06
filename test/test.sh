#!/bin/bash


# Check for 'clean' argument
if [[ "$1" == "clean" ]]; then
    echo "------------------------ RUNNING MAKE CLEAN... ------------------------"
    make clean
    make
fi



# Directories
BIN_DIR="bin"
OBJ_DIR="$BIN_DIR/obj"
SRC_DIR="."
TEST_BINARIES=()


# Check if obj directory is empty or doesn't exist
if [[ ! -d "$OBJ_DIR" || -z "$(ls -A "$OBJ_DIR")" ]]; then
    echo "------------------------ RUNNING MAKE... ------------------------"
    make
fi

# Create necessary directories
mkdir -p "$BIN_DIR"
mkdir -p "$OBJ_DIR"

# Compile and link each test file
echo "Compiling and linking test files..."
find "$SRC_DIR" -type d \( -name "$BIN_DIR" -prune \) -o -type f -name "*.cpp" -print | while read -r file; do
    # Check if the file contains a main function
    if grep -q "int main" "$file"; then
        # Extract the base name of the file (without extension)
        base_name=$(basename "$file" .cpp)
        
        # Define the output binary path
        binary_path="$BIN_DIR/$base_name"
        TEST_BINARIES+=("$binary_path")
        
        # Compile and link the test file with object files from Makefile
        echo "Building $binary_path from $file..."
        g++ "$file" "$OBJ_DIR"/*.o -o "$binary_path" -Wall -Wextra -std=c++17 -I/u/ah7226/COS375-Project-3/src 
    fi
done


echo "------------------------ GENERATION COMPLETE. TESTING... ------------------------"


# Run all binaries in the bin directory
echo "Running all binaries in $BIN_DIR..."
for binary in "$BIN_DIR"/*; do
    if [[ -f "$binary" && -x "$binary" ]]; then  # Check if it's a file and executable
        echo "Running $binary..."
        ./"$binary"
        echo "Finished running $binary."
    fi
done

echo "------------------------ TESTING COMPLETE ------------------------ "
