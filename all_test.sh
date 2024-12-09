#!/bin/bash

# Run all tests in the test directory. Use the 'remove' option to remove test files after running.


# Check if the "remove" option is provided
REMOVE_OPTION=false
if [[ "$1" == "remove" ]]; then
    REMOVE_OPTION=true
fi

failures=0


echo "Running all tests..."

echo "------------------------ RUNNING UNIT TESTS... -------------------------"
cd test
sh test.sh
cd ..   



echo "--------------------- RUNNING INTEGRATION TESTS... ---------------------"



# Initialize a variable to track the last directory
last_dir=""

# Iterate through all .bin files in test/*/ directories
for bin_file in test/*/*.asm; do

    # Extract the directory and base name of the .bin file

    dir=$(dirname "$bin_file")
    base_name=$(basename "$bin_file" .asm)
    
    # Check if the directory has changed
    if [[ "$dir" != "$last_dir" ]]; then
        echo " ================================== COMPLETED BINARY TESTS IN DIRECTORY: $last_dir =================================="
        last_dir="$dir"  # Update last_dir to the current directory
    
        echo " ================================== RUNNING BINARY TESTS IN DIRECTORY: $dir  ==================================" 
        last_dir="$dir"  # Update last_dir to the current directory
    fi

    currentDir=$(pwd)     
    echo "-------------------- ASSEMBLING INTEGRATION TEST... --------------------"
    
    cd bin > /dev/null 2>&1

    if [[ "$base_name" == *"fib"* ]]; then
        echo $(./compile.sh ../"$dir/$base_name" none)
    else
        echo $(./compile.sh ../"$dir/$base_name")
    fi
    
    cd .. > /dev/null 2>&1

    echo "------------------------- FINISHED ASSEMBLING... -----------------------"

    cd "$currentDir" > /dev/null 2>&1

    # Change to the directory containing the binary file
    cd "$dir" || { echo "Error: Could not change to directory $dir"; exit 1; }

    
    # Run sim_cycle with .bin file and cache_config.txt
    ./../../sim_cycle "$base_name.bin" ../cache_config.txt
    
    

    # Construct the expected output file name based on the binary file name
    output_file="${base_name}_cycle_pipe_state.out"
    
    # Check if the expected output file was generated
    if [[ ! -f "$output_file" ]]; then
        echo "Error: $output_file not found for $base_name"
        cd - || exit 1  # Return to previous directory before continuing
        continue
    fi

    # Find reference file matching *_cycle_pipe_state.ref in the same directory as .bin file
    ref_file="${base_name}_cycle_pipe_state.ref"


    
    # Check if reference file exists
    if [[ -f "$ref_file" ]]; then
        # Compare the generated output file with the reference file
        diff -u "$output_file" "$ref_file"
        
        # Check if diff found differences
        if [[ $? -eq 0 ]]; then
            echo "No differences for $bin_file"
        else
            echo "Differences found for $bin_file"
            failures=$((failures + 1))
        fi
    else
        echo "Reference file not found for $base_name"
    fi

    # Conditionally remove "test files"
    if [[ "$REMOVE_OPTION" == true ]]; then
        rm -f *.out *.bin *.elf
        echo "Removed .out files for $base_name"
    fi

    echo "------------------------- FINISHED CYCLE SIM... ------------------------"

    
    # Return to the original working directory before processing the next binary file
    cd - || exit 1 
done

echo " ================================== COMPLETED BINARY TESTS IN DIRECTORY: $last_dir =================================="

echo "--------------------- REGRESSION TESTING COMPLETE ---------------------- "
echo "$failures tests failed!"

