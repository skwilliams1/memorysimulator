The requirements for the project are detailed in the file p2-memsim. To project requires the implementation of a page replacement algorithm that used stored memory addresses from two files:
- bzip.trace.zip
- sixpack.trace.zip

The file to be complied for the program to execute is:
- memsim.cpp: main interface for memory simulator that take user input in two forms

In addition, the folder must contain the trace files along with: 
- fifo.h : this defines the function for the first-in-first-out page replacement policy.
- vms.h : this defines the function for the segmented-fifo page replacement policy.
- lru.h : this defines the function for the least recently used page replacement policy
- Makefile 

The program can be compiled in two ways:
- Using the command “make” which compiles all the files to create the executable memsim
- Using the command “g++ -Wall memsip.cpp -o memsim” which creates the executable memsim

The program can be run using two input formats:
- For FIFO and LRU -  ./memsim < tracefile > < numFrames > < policy>  <quiet/debug>
- For Segmented-FIFO - ./memsim  <tracefile>  <numFrames> <policy> <p> <quiet/debug>

where:
- tracefile is the name of the tracefile being used
- numFrames is the number of frames in page table 
- policy is the scheduling policy being used (LRU/FIFO/VMS)
- quiet/debug determines output. If debug, prints out addresses that causes a disk read. If quiet just prints number of frames, disk writes, - disk reads and in trace
- p is the percentage to be used in calculation of secondary and primary cache sizes (range: 1 to 100)

Examples of output when program is run

Command:
./memsim bzip.trace 64 fifo quiet

Output:
******************************************
Total memory frames: 64
In trace: 1000000
Total disk reads: 1467
Total disk writes: 514
*****************************

Command:
/memsim bzip.trace 64 vms 0 quiet

Output:
******************************************
Total memory frames: 64
In trace: 1000000
Total disk reads: 1467
Total disk writes: 514
******************************************

Command:
./memsim bzip.trace 64 vms 25 quiet

Output:
******************************************
Total memory frames: 64
In trace: 1000000
Total disk reads: 1367
Total disk writes: 471
******************************************

Command:
./memsim bzip.trace 64 vms 100 quiet

Output:
******************************************
Total memory frames: 64
In trace: 1000000
Total disk reads: 1264
Total disk writes: 420
******************************************

Command:
./memsim bzip.trace 64 lru quiet

Output:
******************************************
Total memory frames: 64
In trace: 1000000
Total disk reads: 1264
Total disk writes: 420
******************************************

Example of the program being compiled using make and being run

Command:
make

Output:
g++ -Wall memsim.cpp -o memsim

Command:
./memsim bzip.trace 64 lru quiet

Output:
******************************************
Total memory frames: 64
In trace: 1000000
Total disk reads: 1264
Total disk writes: 420
******************************************
The program was tested and compiled with no warnings under the student cluster
