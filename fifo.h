#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

//This function will create a page table which utilizes FIFO method
//The funtion accepts command line and a trace file from which memory address are read and added to page table utilizing FIFO method 

void FIFO(char *argv[], FILE *tf) {
    unsigned int numFrames = atoi(argv[2]);  //holds number of frames which was passed as the third agrument in the command line
    int inTrace = 0;    //define in trace
    int diskReads = 0;  //counts the number of disk reads that happens
    int diskWrites = 0; //counts the number of disk writes that happens
    int page_address; //holds the actual page address 
    unsigned addr = 0;  //stores the address from trace file
    char rw;   //stores R or W from trace file
    string mode = argv[4]; //stores whether code is in debugging mode or quiet mode

    vector <pair <int, char> > fifoPT; //creates a vector that will serve as the fifo page table, each element will be a pair of page address and read or write
 
    
    while(fscanf(tf,"%x %c",&addr, &rw) != EOF) {

        bool found = false; //this is used to determine if a page has been found in the page table

        page_address = addr/4096; //this calculates the actual value of the page address

        inTrace++; //This counts the number of memory adddress being read from the trace file

        //this for loop searches each element in the page table to check if the address read from the file is already in the page table
        for (unsigned int i = 0; i < fifoPT.size(); i++){

            //this checks if the page address which is the first element in the pair of element in page table matches the page address being searched for
            if (fifoPT.at(i).first == page_address) {

                found = true; //if found the found variable is changed to true

                //if page has a read bit and rw is write then update rw bit of page in page table to write
                if ((fifoPT.at(i).second == 'r' || fifoPT.at(i).second == 'R') && (rw == 'w' || rw == 'W')){

                    fifoPT.at(i).second = 'w';
                }
                break;
            }
        }

        //this checks if page was found in page table and skips to reading next line from trace file if it was found
        if (found == true) {

            continue;
        }

        //check to see if page table is full, if yes remove first entry and increase disk write if W then add new page address to back
        if (fifoPT.size() == numFrames) {

            if (fifoPT.at(0).second == 'W' || fifoPT.at(0).second == 'w' ) {
                diskWrites++;
            }
            diskReads++;
            fifoPT.erase(fifoPT.begin());
            fifoPT.push_back(make_pair(page_address,rw));
    
        }

        //if page not found and page table not full just add to back of page table and increment disk reads
        else { 

            fifoPT.push_back(make_pair(page_address, rw));
            diskReads++;

             //If in debug mode prints out page address and rw bit that has caused a disk read
            if (mode == "debug") {

                cout << "********DEBUG DISK READ **********" << endl;
                cout << "Page Address: " << page_address << endl;
                cout << "Read or Write: " << rw << endl;
            }
             
        }
    }
        
    cout << "******************************************" << endl;
    cout << "Total memory frames: " << numFrames << endl; 
    cout << "In trace: " << inTrace << endl;
    cout << "Total disk reads: " << diskReads << endl;
    cout << "Total disk writes: " << diskWrites << endl;
    cout << "******************************************" << endl;
} 

/*int main (int argc, char *argv[]) {

    char* in = argv[1];
    FILE* tf = fopen(in,"r");

    FIFO(argv, tf);

   
    fclose(tf);

    return 0;



}*/
