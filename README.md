# Final Project of Algorithms and Data Structures  
### Simulator for non-deterministic Turing Machines 

Implementation in C language of an interpreter for non-deterministic Turing Machines, considering the variant with single tape and only accepting states.

The input file first provides the transition function, then the acceptence states and the maximum number of steps for a single computation (in order to avoid machines that do not end). Finally several strings for the machine to be read.

The ouput is a file that tells for each string if it is accepted or not (1 or 0) and eventually U in case the limit of the number of steps is exceeded.
