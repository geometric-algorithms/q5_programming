#!/bin/bash
set -e  # Exit immediately if any command fails

# Compile the C++ program
g++ a.cpp -o a.out

# Run the program with input from in.txt and output to out.txt
./a.out < in.txt > out.txt

# Generate the plot using Python
python3 plot.py

echo "Test completed successfully!"
