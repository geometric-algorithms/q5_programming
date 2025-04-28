#!/bin/bash
set -e  # Exit immediately if any command fails

# Compile the C++ program
g++ b.cpp -o b.out

# Run the program with input from in.txt and output to out.txt
./b.out < in.txt
