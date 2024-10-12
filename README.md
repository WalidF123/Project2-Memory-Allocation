## Project Overview
This project implements two memory allocation strategies, First Fit and Best Fit, using dynamic memory management with sbrk() and linked lists to track allocated and free memory blocks.

The programs allow memory allocation and deallocation based on the specified strategy and print the status of the allocated and free memory blocks.

First Fit: Finds the first memory block large enough to fulfill the request.

Best Fit: Finds the smallest memory block that fits the request, minimizing leftover free space.
Files Included

firstfit.cpp: Implements the First Fit memory allocation strategy.

bestfit.cpp: Implements the Best Fit memory allocation strategy.

Makefile: Used to compile the two programs.
Test data file: You can use a custom text file with commands to test the program (instructions below).

## NOTE:
- In order to run on the school servers you need to have the command 'scl enable devtoolset-11 zsh' prior to running the program.

## Makefile:
The Makefile is provided to build both programs (firstfit and bestfit). To compile the programs, simply run the following command:

  - make all
  - # This will generate two executable files:

  firstfit: Implements the First Fit memory allocation strategy.
  
  bestfit: Implements the Best Fit memory allocation strategy.
To clean up the generated files (object files and executables), run:
  -   make clean

## Running the Programs
To run the First Fit program:

 - ./firstfit datafile

 
To run the Best Fit program:

 -   ./bestfit datafile

Group Members: 
Walid Feki s3972717
Anthony Vo s3951749
