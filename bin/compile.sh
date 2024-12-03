#!/bin/bash

# Check if the first argument is provided
if [ -z "$1" ]; then
    echo "Usage: $0 <filename>"
    exit 1
fi

FILENAME="$1"

# Assemble the .asm file into an .elf file
./mips-linux-gnu-as "../test/$FILENAME.asm" -o "../test/$FILENAME.elf"

# Copy the .text section from the .elf file to a binary file
./mips-linux-gnu-objcopy "../test/$FILENAME.elf" -j .text -O binary "../test/$FILENAME.bin"

echo "Compilation and conversion successful for $FILENAME."