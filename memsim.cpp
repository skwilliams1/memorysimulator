#include "fifo.h"
#include "lru.h"
#include "vms.h"

int main (int argc, char *argv[]) {

    char* in = argv[1];
    FILE* tf = fopen(in,"r");
    string pagingMethod = argv[3];
    

    if (pagingMethod == "vms") {
       VMS(argv, tf);
    }
    else if (pagingMethod == "fifo") {

        FIFO(argv,tf);
    }
    else if (pagingMethod == "lru") {

        LRU(argv,tf);
    }

    
   
    fclose(tf);

    return 0;



}