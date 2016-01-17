Eric Rock
CS 510 - 001
1/12/2016
Assignment 1 - Sliding Block Puzzle Solver / Random Walk, Uninformed Search


=============================
======  DOCUMENTATION  ======
=============================

Along with this README, a PDF report containing the answers to the written
portion of Assignment 1 is located within doc/report.pdf. Most header files
contain suitable documentation above function prototypes, with some comments
included inline with source code in a much less regular fashion.

=============================
======      SOURCE     ======
=============================

The source for this project is located in the src/ directory. Miscellaneous
header and source files have been included, which provide miscellaneous helper
functionality, such as data structures, I/O functionality, and string
processing. 

These miscellaneous headers include:
sk_types/*      - Commonly used types, function classes
sk_enum/*       - Enumeration definition macro.
sk_list/*       - Linked list data structure
sk_vector/*     - Dynamically resizing array data structure
sk_iterator/*   - Iterator data structure
sk_tree/*       - Tree data structure
sk_map/*        - Map data structure
sk_hash_table/* - Hash Table data structure
sk_str/*        - String data structure
sk_random/*     - Random number generator interface
preprocessor/*  - Preprocessor magic. Seriously, don't look in these files.
utils.h         - memory management and miscellaneous definitions
sk_clargs.h     - Command line argument processing algorithms
printer.h       - Common output delegate structure

The main "driver" components of this project are:
src/main.c              - main driver. Perfororms program configuration, input
                            processing, solution derivation, and program output.
src/sk_iterator_utils.c - Provides iterators for some c-type static structures.

=============================
=======     BUILD     =======
=============================

The default target is sufficient to build the project, so just run this command:
    make

in the same directory as this README.

A 'clean' target has been provided to clean up object files and the executable.
    make clean

=============================
========     RUN     ========
=============================

The 'run' target has been provided to run the built executable.
    make run

The default execution of the program will execute a random walk of 3 moves over
the input file assets/SBP-level0.txt.

The following command line options are recognized:
    -r <# moves>
        Perform a random walk of the given number of moves across the board.
        Ex: ./sbp -r 50
    -d
        Perform a depth first search for a solution.
        Ex: ./sbp -d
    -b
        Perform a breadth first search for a solution.
        Ex: ./sbp -b
    -f <file path>
        Use the file at the given path (relative or absolute) as the starting
        board configuration.
        Ex: ./sbp assets/SBP-level1.txt

It is worthwhile to note that the separate search strategies are mutually
exclusive. Attempting to supply more than one of -r, -d, or -b at a time will
not be accepted as a valid program configuration, and the program will abort
with an error message.

=============================
=== IMPLEMENTATION STATUS ===
=============================

Of the features specified in the project description, the following are
implemented fully. This claim is made in good faith, and all test cases provided
via the program specification have been verified. It is possible that some
corner cases may have been missed, but I have put in an effort to try to root
out as many as I can.

STATE REPRESENTATION        IMPLEMENTED
PUZZLE COMPLETE CHECK       IMPLEMENTED
MOVE GENERATION             IMPLEMENTED
STATE COMPARISON            IMPLEMENTED
NORMALIZATION               IMPLEMENTED
RANDOM WALKS                IMPLEMENTED
UNINFORMED SEARCH           IMPLEMENTED

=============================
========== TESTING ==========
=============================


The following test cases were used to test the validity of this program:

./sbp -r 10 -f assets/SBP-level0.txt
./sbp -r 10 -f assets/SBP-level1.txt
./sbp -r 10 -f assets/SBP-level2.txt
./sbp -r 10 -f assets/SBP-level3.txt
./sbp -d -f assets/SBP-level0.txt
./sbp -d -f assets/SBP-level1.txt
./sbp -d -f assets/SBP-level2.txt
./sbp -d -f assets/SBP-level3.txt
./sbp -b -f assets/SBP-level0.txt
./sbp -b -f assets/SBP-level1.txt
./sbp -b -f assets/SBP-level2.txt
./sbp -b -f assets/SBP-level3.txt

