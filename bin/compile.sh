#!/bin/bash

# Check if the first argument is provided
if [ -z "$1" ]; then
    echo "Usage: $0 <file path without extension>"
    exit 1
fi

FILEPATH="$1"

# Extract the base name and check for extensions
BASENAME=$(basename "$FILEPATH")  # Get the last part of the path (e.g., "myfile")
if [[ "$BASENAME" == *.* ]]; then
    echo "Error: Please do not include a file extension in the file path."
    exit 1
fi

# Assemble the .asm file into an .elf file
./mips-linux-gnu-as "${FILEPATH}.asm" -o "${FILEPATH}.elf"

# Copy the .text section from the .elf file to a binary file
./mips-linux-gnu-objcopy "${FILEPATH}.elf" -j .text -O binary "${FILEPATH}.bin"

echo "Compilation and conversion successful for $FILEPATH."
