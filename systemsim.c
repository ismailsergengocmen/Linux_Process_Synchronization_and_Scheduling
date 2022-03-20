#include "systemsim.h"
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>

static void* process_generator(void* param) {
    int process_count = 1;
    pthread_t tid;
    pthread_attr_t attr;

    int count;
    if (ALLP < 10 && ALLP < MAXP) {
        count = ALLP;
    } else if (ALLP < 10 && ALLP >= MAXP) {
        count = MAXP;
        } else if (ALLP >= 10) {
        count = 10;
    } else {
        count = 0;
    }

    for (int i = 0; i < count; i++) {
        //pthread_attr_init (&attr);
        //pthread_create (&tid, &attr, process_generator, NULL); // BURAYI UPDATELE
        struct timeval start;
        gettimeofday(&start, NULL);

        struct PCB* temp = (struct PCB*)malloc(sizeof(struct PCB));
        temp->pid = process_count;
        temp->tid = tid;
        temp->state = 1;
        temp->arrival_time = start.tv_usec * (0.001);
        process_count++;
        live_process_count++;
        total_process_count++;
    }

    srand(time(NULL));

    while (1) {
        usleep(5000);
        double random = (double)rand() / RAND_MAX;

        if (random <= pg) {
            if (total_process_count < ALLP && live_process_count < MAXP) {
                printf("CREATED, %f: \n", random);
                //pthread_attr_init (&attr);
                //pthread_create (&tid, &attr, process_generator, NULL); // BURAYI UPDATELE
                struct timeval start;
                gettimeofday(&start, NULL);

                struct PCB* temp = (struct PCB*)malloc(sizeof(struct PCB));
                temp->pid = process_count;
                temp->tid = tid;
                temp->state = 1;
                temp->arrival_time = start.tv_usec * (0.001);
                process_count++;
                live_process_count++;
                total_process_count++;
            } else if (total_process_count == ALLP) {
                pthread_exit(0);
            }
        }
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

    //pthread_t tid;
    //int a = pthread_create (&tid, NULL, process_generator, NULL); // BURAYI UPDATELE
    //pthread_join(tid, NULL);

    return 0;
}