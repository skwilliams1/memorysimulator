#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>                     
#include <fstream>



using namespace std;

void VMS (char *argv[], FILE* tf) {
    unsigned int numFrames = atoi(argv[2]);  //holds number of frames which was passed as the third agrument in the command line
    int inTrace = 0;    //define in trace
    int diskReads = 0;  //counts the number of disk reads that happens
    int diskWrites = 0; //counts the number of disk writes that happens
    int page_address;
    unsigned addr = 0;  //stores the address from trace file
    char rw;   //stores R or W from trace file
    int tempPageAddr; //store page address temporarily when adding from fifo to lru
    char tempRW;  //stores rw bit temporarily when adding from fifo to lru 
    unsigned int lruCache_size; //used to store size of lru cache which is the secondary cache
    unsigned int fifoCache_size; //used to store size of fifo cache which is the primary cache
    int p = atoi(argv[4]);
    bool lruUnavailable = false; // this variable will determine if lru is using any of the memory, that is p = 1;
    bool fifoUnavailable = false; // this variable will determine if fifo is using any memeory, that is p = 100
    string mode = argv[5]; //stores whether code is in debugging mode or quiet mode

    //cout << argv[4] << endl;

    
    //deals with edge cases of p = 100 and p = 1, which will make one of the caches unavailable
    if (p <= 1 ){

        lruUnavailable = true;

    }
    if (p == 100) {
        
        fifoUnavailable = true;
    }

    //calculates size of lru and fifo cache
    lruCache_size = (numFrames * p) / 100; 
    fifoCache_size = numFrames - lruCache_size;



    vector <pair <int, char> > fifoCache; //creates fifo page table using a vector of pairs
    vector <pair <int, char> > lruCache; //creates lru page table using a vector of pairs

    while(fscanf(tf,"%x %c",&addr, &rw) != EOF) {

        
        page_address = addr/4096;  //calculate actual page address
        bool found = false;  //used to determine if page has been found in table
        inTrace++; //counts the number of memory addresses being read from a trace file

    
        //checks if fifo is available before searching
        if (!fifoUnavailable) {

            //searches for address in fifo cache and updates rw bit if necessary
            for (unsigned int i = 0; i < fifoCache.size(); i++){

                 //this checks if the page address which is the first element in the pair of element in page table matches the page address being searched for
                if (fifoCache.at(i).first == page_address) {

                    found = true; //found variable updated to true

                    //if page has a read bit and rw is write then update rw bit of page in page table to write
                    if ((fifoCache.at(i).second == 'r' || fifoCache.at(i).second == 'R') && (rw == 'w' || rw == 'W')){

                        fifoCache.at(i).second = 'w';
                        
                    }
                    break; //breaks from for loop
                    
                }
            }
        } 

        //if found in fifo cache then moves on to next line in trace file 
        if (found == true) {

            continue;
        }

        //checks if lru is available before searching it 
        if (!lruUnavailable) {
            //searches for address in lru cache if found update rw bit if necessary
            for (unsigned int i = 0; i < lruCache.size(); i++){
                
                if (lruCache.at(i).first == page_address) {
                    
                    found = true;

                    //checks if page in page table has write bit and if rw is r then updates rw to be write
                    if ((lruCache.at(i).second == 'w' || lruCache.at(i).second == 'W') && (rw == 'r' || rw == 'R')) {
                    
                        rw = 'w';
    
                    } 

                    lruCache.erase(lruCache.begin()+ i); //remove page that was found from lru cache

                    //if fifo is available we will have to move first page from fifo cache to back of lru cache and move found page to back of fifo
                    if (!fifoUnavailable) { 
                    
                        //stores the first page in fifo information in temporary variables
                        tempPageAddr = fifoCache.at(0).first;
                        tempRW = fifoCache.at(0).second;

                        lruCache.push_back(make_pair(tempPageAddr,tempRW)); //add front of fifo cache to back of lru cache
                        fifoCache.erase(fifoCache.begin());  //remove front of fifo cache to make space for element from lru cache
                        fifoCache.push_back(make_pair(page_address, rw)); //add found page with write bit to back of fifo cache
                    }
                    //if it is not available just move found element to back of lru cache 
                    else {

                        lruCache.push_back (make_pair(page_address, rw));
                    }
                    break;
                    
                }
            }
        } 

        //if found in lru cache continues to next line in trace file
        if (found == true) {

            continue;
        }

        //if element not found we will have to add it to the fifo cache or lru cache 
        //if fifo cache is not full we add to it to fifo since it is our primary cache 
        //if fifo cachse is full we add it to lru since it is our secondary cache 
        //if both are full we remove front of lru and move front of fifo to lru and add new page to back of fifo

        //checks if fifo cache is full
        if (!(fifoCache.size() >= fifoCache_size) && !fifoUnavailable) {

            fifoCache.push_back(make_pair(page_address, rw));
            diskReads++;
            
            //If in debug mode prints out page address and rw bit that has caused a disk read
            if (mode == "debug") {

                cout << "********DEBUG DISK READ **********" << endl;
                cout << "Page Address: " << page_address << endl;
                cout << "Read or Write: " << rw << endl;
            }

        } 

        //if fifo cache is full we will check if lru cache is full
        else if (!(lruCache.size() >= lruCache_size) && !lruUnavailable) {

            //if fifo available move front of fifo to back of lru and add new page to back of lru
            if (!fifoUnavailable) {
                tempPageAddr = fifoCache.at(0).first;
                tempRW = fifoCache.at(0).second;
                lruCache.push_back(make_pair(tempPageAddr, tempRW));
                fifoCache.erase(fifoCache.begin());
                fifoCache.push_back(make_pair(page_address,rw));
            }

            //if not available just add new element to back
            else {
                lruCache.push_back(make_pair(page_address,rw));
            }
            diskReads++;

            //If in debug mode prints out page address and rw bit that has caused a disk read
            if (mode == "debug") {

                cout << "********DEBUG DISK READ **********" << endl;
                cout << "Page Address: " << page_address << endl;
                cout << "Read or Write: " << rw << endl;
            }

            
        }

        //if both are full we will carry out necessary process
        else {

            //if fifo is unavailbe and lru is full remove front of lru and add new page to back
            if (fifoUnavailable) {

                //checks to see if page being removed has a write bit
                if (lruCache.at(0).second == 'w' || lruCache.at(0).second == 'W') {

                    diskWrites++;
                }
                lruCache.erase(lruCache.begin());
                lruCache.push_back(make_pair(page_address, rw));
                diskReads++;
                
                //If in debug mode prints out page address and rw bit that has caused a disk read
                if (mode == "debug") {

                    cout << "********DEBUG DISK READ **********" << endl;
                    cout << "Page Address: " << page_address << endl;
                    cout << "Read or Write: " << rw << endl;
                }

                continue;
            }

            //if lru is unavailable and fifo is full remove front of fifo and add new page to back
            if (lruUnavailable) {

                //checks to see if page being removed has a write bit
                if (fifoCache.at(0).second == 'w' || fifoCache.at(0).second == 'W') {

                    diskWrites++;
                }
                fifoCache.erase(fifoCache.begin());
                fifoCache.push_back(make_pair(page_address, rw));
                diskReads++;
                
                //If in debug mode prints out page address and rw bit that has caused a disk read
                if (mode == "debug") {

                    cout << "********DEBUG DISK READ **********" << endl;
                    cout << "Page Address: " << page_address << endl;
                    cout << "Read or Write: " << rw << endl;
                }

                continue;
            }

            //if both are avaialbe then we erase from front of lru and move front of fifo to back of lru and new page to back of fifo 

            //checks to see if the page being removed has a write bit
            if (lruCache.at(0).second == 'w' || lruCache.at(0).second == 'W') {

                diskWrites++;
            }
            lruCache.erase(lruCache.begin());

            //stores the first page in fifo information in temporary variables
            tempPageAddr = fifoCache.at(0).first;
            tempRW = fifoCache.at(0).second;

            lruCache.push_back(make_pair(tempPageAddr,tempRW)); 
            fifoCache.erase(fifoCache.begin());
            fifoCache.push_back(make_pair(page_address,rw));
            diskReads++;
            
            //If in debug mode prints out page address and rw bit that has caused a disk read
            if (mode == "debug") {

                cout << "********DEBUG DISK READ**********" << endl;
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
