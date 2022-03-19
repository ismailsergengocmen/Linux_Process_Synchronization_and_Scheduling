#include "queue.c"
#include "systemsim.h"
#include <stdlib.h>

int main(int argc, char** argv) {
    char* ALG = argv[1];

    if(strcmp(ALG, "RR") == 0){
        long long Q = strtoll(argv[2], NULL, 10);
    }
    else {
        char* Q = argv[2];
    }
  
    int T1 = atoi(argv[3]);
    int T2 = atoi(argv[4]);
    char* burst_dist = argv[5];
    long long burstlen = strtoll(argv[6], NULL, 10);
    long long min_burst = strtoll(argv[7], NULL, 10);
    long long max_burst = strtoll(argv[8], NULL, 10);
    double p0 = strtod(argv[9], NULL);
    double p1 = strtod(argv[10], NULL);
    double p2 = strtod(argv[11], NULL);
    double pg = strtod(argv[12], NULL);
    int MAXP = atoi(argv[13]);
    int ALLP = atoi(argv[14]);
    int OUTMODE = atoi(argv[15]);

    



    return 0;
}
