#!/bin/bash

# Check if the first argument (file path) is provided
if [ -z "$1" ]; then
    echo "Usage: $0 <file path without extension> [none]"
    exit 1
fi

FILEPATH="$1"

# Extract the base name and check for extensions
BASENAME=$(basename "$FILEPATH")  # Get the last part of the path (e.g., "myfile")
if [[ "$BASENAME" == *.* ]]; then
    echo "Error: Please do not include a file extension in the file path."
    exit 1
fi

# Check if the second argument is "none"
if [[ "$2" == "none" ]]; then
    MARCH_FLAG=""
else
    MARCH_FLAG="-march=r4000"
fi

# Assemble the .asm file into an .elf file
./mips-linux-gnu-as "${FILEPATH}.asm" -o "${FILEPATH}.elf" $MARCH_FLAG

# Copy the .text section from the .elf file to a binary file
./mips-linux-gnu-objcopy "${FILEPATH}.elf" -j .text -O binary "${FILEPATH}.bin"

echo "Compilation and conversion successful for $FILEPATH."