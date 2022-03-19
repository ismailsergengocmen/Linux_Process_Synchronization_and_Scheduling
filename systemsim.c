#include "queue.c"
#include "systemsim.h"
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
    ALG = argv[1];

    if(strcmp(ALG, "RR") == 0){
        Q1 = strtoll(argv[2], NULL, 10);
    }
    else {
        Q2 = argv[2];
    }
  
    T1 = atoi(argv[3]);
    T2 = atoi(argv[4]);
    burst_dist = argv[5];
    burstlen = strtoll(argv[6], NULL, 10);
    min_burst = strtoll(argv[7], NULL, 10);
    max_burst = strtoll(argv[8], NULL, 10);
    p0 = strtod(argv[9], NULL);
    p1 = strtod(argv[10], NULL);
    p2 = strtod(argv[11], NULL);
    pg = strtod(argv[12], NULL);
    MAXP = atoi(argv[13]);
    ALLP = atoi(argv[14]);
    OUTMODE = atoi(argv[15]);

    return 0;
}
