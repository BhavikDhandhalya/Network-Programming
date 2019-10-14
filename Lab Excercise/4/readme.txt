## Problem Statement
Let us analyse the performance of writing larage data to a file using mmap() vs write() call. You need to
write at least 1 GB of data into a file. You program should print time taken for both mmap() and write()
call.


------------------------------

I am reading 1GB file and making 2 duplicate same file using write and mmap.
to generate 1GB file,

first compile & run "generatefile.c".

It will generate 1GB file with name "GB".

later you can pass this file in command line arguments as input file.


Later to check correctness of the program,
you can use "diff" command.
