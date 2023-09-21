#include <vector>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

//This function will create a page table which utilizes LRU method
//The funtion accepts command line and a trace file from which memory address are read and added to page table utilizing LRU method 

void LRU (char *argv[], FILE* tf) {

    unsigned int numFrames = atoi(argv[2]);  //holds number of frames which was passed as the third agrument in the command line
    int inTrace = 0;    //stores number of address read from file
    int diskReads = 0;  //counts the number of disk reads that happens
    int diskWrites = 0; //counts the number of disk writes that happens
    int page_address; //holds the actual page address 
    unsigned addr = 0;  //stores the memory address from trace file
    char rw;   //stores R or W from trace file 
    string mode = argv[4]; //stores whether code is in debugging mode or quiet mode
   


    vector<pair<int ,char> > lruPT; //creates a vector that will serve as the lru page table, each element will be a pair of page address and read or write
 
    
    
    while(fscanf(tf,"%x %c",&addr, &rw) != EOF) {

        

        bool found = false; //this is used to determine if a page has been found in the page table

        page_address = addr/4096; //this calculates the actual value of the page address

        inTrace++; //This counts the number of memory adddress being read from the trace file

        //this for loop searches each element in the page table to check if the address read from the file is already in the page table
        for (unsigned int i = 0; i < lruPT.size(); i++){

            //this checks if the page address which is the first element in the pair of element in page table matches the page address being searched for
            if (lruPT.at(i).first == page_address) {
                found = true; //if found the found variable is changed to true

                //checks if page in page table has write bit and if rw is r then updates rw to be write
                if ((lruPT.at(i).second == 'w' || lruPT.at(i).second == 'W') && (rw == 'r' || rw == 'R')) {
                    
                    rw = 'w';
    
                } 
                
                //page is removed from lru page table and added to the back of lru page table to show its been recently used
                lruPT.erase(lruPT.begin() + i);
                lruPT.push_back(make_pair(page_address, rw));
                break;

            }
        }

        //this checks if page was found in page table and skips to reading next line from trace file if it was found
        if (found == true) {
            
            continue;
        }
        //check to see if page table is full, if yes remove first entry and increase disk write if W then add new thing to back
        if (lruPT.size() == numFrames) {

            

            if (lruPT.at(0).second == 'W' || lruPT.at(0).second == 'w' ) {
               
                diskWrites++;
            }
            //removing least recently used page from page table which is at the front of vector and adds the new page to the back 
            diskReads++;
            lruPT.erase(lruPT.begin());
            lruPT.push_back(make_pair(page_address,rw));
    
        }
        //if page table is not full then just add new page to back of page table
        else { 
            
             lruPT.push_back(make_pair(page_address, rw));
             diskReads++; //disk read occurs because page not in table 

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
