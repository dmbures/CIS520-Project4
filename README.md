Program description:
We are interested in the "scorecard" that we can find in large text files. On Beocat there is a moderately large (wiki_dump.txt, 1.7GB) file containing approximately 1M Wikipedia entries, 1 entry per line. The file location is ~dan/625/wiki_dump.txt (use this file – do not make your own copies of the data files). You can also find sample programs in ~dan/625.

Read the file into memory, find the maximum value of ASCII character numeric values for all the characters in a line. So, if the first line is "ab" and the second is "bc" then val(a) = 97; b = 98; c = 99; etc. (from https://en.wikipedia.org/wiki/ASCII#Printable_characters)Links to an external site. then your output for lines 0 and 1 would be 98 and 99, respectively.

Print out a list of lines, in order, with the line number and maximum, e.g.

0: 98
1: 99
etc.

Your output should be identical for all versions of your code. There is a serial program called ~dan/625/simple_avg_chars.c which calculates the average of the character values in a line (feel free to use it as a reference).

Mechanics:
Your first task is to implement your solution using pthreads. Your second task is to do a performance evaluation across a range of input sizes and core counts to see how the various versions match up in terms of run times, CPU efficiency, memory utilization, etc. You'll want to keep the machines constant, so use the Slurm flag to confine your jobs to only the "mole" class nodes (https://support.beocat.ksu.edu/Docs/Compute_Nodes#MolesLinks to an external site.). Use a reasonable number of cores – up to 40 on Beocat. Show how performance differs (if it does) across multiple machines vs. a single machine (you will only be able to do this up through 20 cores).
Graph your performance results (include the graphs in your final design document). Discuss them. Are there any race conditions? How do you handle synchronization between processes? How much communication do you do, and are you making any attempts to optimize this? Why or why not?
Repeat step 1 for MPI
Repeat step 1 for OpenMP
WARNING: The 1 core/60MB data elements version of the code can take a while (hours) to run, so start early!

Resources:
pthreads – https://hpc-tutorials.llnl.gov/posix/Links to an external site. 
MPI – https://hpc-tutorials.llnl.gov/mpi/Links to an external site.
OpenMP – https://hpc-tutorials.llnl.gov/openmp/Links to an external site. 
Generic technical paper outline – https://cse.unl.edu/~goddard/WritingResources/Links to an external site.
For help using the Beocat scheduler and its command line options, see the general help page – https://support.beocat.ksu.edu/BeocatDocs/index.php/Main_PageLinks to an external site.
Scheduler help page – https://support.beocat.ksu.edu/BeocatDocs/index.php/SlurmBasicsLinks to an external site.
Compute node specifications – https://support.beocat.ksu.edu/BeocatDocs/index.php/Compute_NodesLinks to an external site.
Beocat introductory videos – https://www.youtube.com/channel/UCfRY7ZCiAf-EzEqJXEXcPrwLinks to an external site.
Performance Analysis for Beginners
