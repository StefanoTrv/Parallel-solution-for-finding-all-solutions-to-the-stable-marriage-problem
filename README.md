# The problem
The goal is to generalize the classic version of the [stable marriage problem](https://en.wikipedia.org/wiki/Stable_marriage_problem). In particular, given an integer ***n*** and two preference lists (one for men and the other for women), we want to enumerate all possible stable matchings for that instance.

# Written in
This project is written in C for the serial part, CUDA for the parallel version and R for the part ralated with time analysis.

# How to use
>   [!IMPORTANT]
>   Makefile is written for Windows systems. This means that `make` commands probably doesn't work on other systems, such as UNIX systems.

- ***Specific instances***:
  To test specific instances, you can pass an input file (.txt) to the program and specify the path of output. Examples of input file can be found in *`\tests\`*. So, the commands for both serial and parallel version are something like 

  `make serial input_file=... output_file=...`

  `make serial input_file=... output_file=...` 

- ***Multiple random-generated instances***:
  Multiple random-generated instances can be tested, by passing instances dimension, number of iterations to test, mode which can be ***classic*** or ***times*** for time analysis purposes. Finally, output path must be provided. Note that the seed can be omitted and if not specified, a default seed is used. Commands for the two versions are

  `make tester instance=... iterations=... mode=... output_file=... seed=...`

  `make cuda_tester instance=... iterations=... mode=... output_file=... seed=...`

- ***Time analysis***:
  Time analysis can be also performed, with ***pre-defined*** parameters that can be directly modified in Makefile. Output files are saved in *`\src\output\serial_tests\`* and in *`\src\output\parallel_tests\`*. Commands are

  `make serial_analysis`

  `make parallel_analysis`

- ***Versions comparison***:
  A comparison of results given by both versions can be carried out. Just instances dimension and number of iterations must be provided. The command is
  
  `make comparator instance=... iterations=...`

- ***Demo***:
  A demo of the program with specific arguments can be launched by the following command

  `make demo`
