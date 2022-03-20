#include "systemsim.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

void calculateNewCpuBurst(struct PCB* pcb, char* burst_dist, int burstlen, int min_burst, int max_burst){
    srand(time(NULL));
    if(strcmp(burst_dist,"fixed") == 0){
        pcb->next_cpuburst_len = burstlen;
    }
    else if(strcmp(burst_dist,"uniform") == 0){
        pcb->next_cpuburst_len = (double) rand() / RAND_MAX * (max_burst - min_burst) + min_burst;
    }
    else{
        double lambda = (double) (1.0 / burstlen);
        double u = (double) rand() / RAND_MAX;
        double x = ((-1) * log(u)) / lambda;

        while(!(min_burst <= x && x <= max_burst)) {
            u = (double) rand() / RAND_MAX;
            x = ((-1) * log(u)) / lambda;
        }
        pcb->next_cpuburst_len = x;
    }
}

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

    struct Queue* a = createQueue();



    return 0;
}