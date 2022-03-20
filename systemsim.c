#include "systemsim.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>

void calculateNewCpuBurst(struct PCB* pcb){
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

static void* process_generator(void* param) {
  
    pthread_t tid;

    int count;
    if (ALLP < 10 && ALLP < MAXP) {
        count = ALLP;
    } 
    else if (ALLP < 10 && ALLP >= MAXP) {
        count = MAXP;
    } 
    else if (ALLP >= 10) {
        count = 10;
    } 
    else {
        count = 0;
    }

    for (int i = 0; i < count; i++) {
        struct PCB* pcb = (struct PCB*)malloc(sizeof(struct PCB));
        pthread_create (&tid, NULL, processThread, pcb); 
        
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
                struct PCB* pcb = (struct PCB*)malloc(sizeof(struct PCB));
                pthread_create (&tid, NULL, processThread, pcb); 

                process_count++;
                live_process_count++;
                total_process_count++;
            } else if (total_process_count == ALLP) {
                pthread_exit(0);
            }
        }
    }
}

static void* cpu_scheduler(void* param) {
    
    while(1){
        sem_post(&S);
        pthread_cond_wait(&scheduler, &lock);
        if(ALG == "FCFS"){
            struct PCB temp = deQueue(CPU.queue);
            temp.state = "RUNNING";
            CPU.pcb = &temp;  
            pthread_cond_broadcast(&CPU.cv);
        }

        else if(ALG == "SJF"){

        }

        else {  // RR

        }
    }
}
             
static void* processThread(struct PCB* pcb){

    struct timeval start;
    gettimeofday(&start, NULL);

    // Filling information 
    pcb->pid = process_count;
    pcb->tid = pthread_self();
    pcb->arrival_time = start.tv_usec * (0.001);
    pcb->rem_cpuburst_len = pcb->next_cpuburst_len;
    pcb->num_cpuburst = 0;
    pcb->time_spend_ready = 0;
    pcb->device1_io_count= 0;
    pcb->device2_io_count = 0;
    pcb->finish_time = 0;
    pcb->total_exec_time = 0;

    srand(time(NULL));

    // LIFE OF A THREAD //
    while(1){ 
        calculateNewCpuBurst(pcb); // Calculate next cpu burst

        enQueue(CPU.queue, *pcb); // Added to ready queue  // TO DO => Scheduler  |  4 , 5  |
        pcb->state = "READY";

        sem_wait(&S);
        pthread_cond_signal(&scheduler); // TO DO => semaphore
        sem_post(&S);

        // CPU //
        while(pcb->tid != CPU.pcb->tid){
            pthread_cond_wait(&CPU.cv, &lock); // Add itself to CPU.cv.ready queue and sleep
        }

        // deQueue_tid(CPU.queue,pcb->tid); // PCB dequeued from ready queue
        // CPU.pcb = pcb; // Put pcb to CPU

        // TO DO => Add RR check and change usleep time accordingly   | 3 |
        usleep(pcb->next_cpuburst_len * 1000);  // If it wakes, it means it's in CPU so it will usleep
        CPU.pcb = NULL; // CPU emptied

        int chance = (double) rand() / RAND_MAX;

        int minInterval = p0;
        int midInterval = p0 + p1;
        
        if(chance < minInterval){ // Terminate    
            enQueue(TERMINATED,*pcb); // PCB added to terminated queue    // TO DO => Scheduler  |  1  |
            pcb->state = "TERMINATED";
            pthread_exit(0); // Thread ended
        }

        else if(minInterval < chance && chance < midInterval) { //IO1
            if(IO1.count == 0 && IO1.pcb == NULL){ // If IO1 is empty and there is no one waiting
                pcb->state = "USING DEVICE1";
                IO1.pcb = pcb; // IO1 has chosen thread
                usleep(T1 * 1000); // Operates on I01 for t1 milisecond
                pcb->device1_io_count += 1;
                pthread_cond_signal(&IO1.cv); // wakes random thread from I01.cv's ready queue
                IO1.pcb = NULL;
            }
            else {
                IO1.count += 1; // Increment IO1 ready queue waiters count 
                enQueue(IO1.queue, *pcb);  // TO DO => Scheduler  |  2  |
                pcb->state = "WAITING DEVICE1";
                pthread_cond_wait(&IO1.cv, &lock); // Thread put in IO1 ready queue(waiting queue)
                deQueue_tid(IO1.queue, pcb->tid);

                pcb->state = "USING DEVICE1"; 
                IO1.count -= 1; // Lower IO1 ready queue waiters count 
                IO1.pcb = pcb; // IO1 has chosen thread
                usleep(T1 * 1000); // Operates on I01 for t1 milisecond
                pcb->device1_io_count += 1;
                pthread_cond_signal(&IO1.cv); // wakes random thread from I01.cv's ready queue
                IO1.pcb = NULL; // I01 is empty now
            }
        }

        else{ // IO2
            if(IO2.count == 0 && IO2.pcb == NULL){ // If IO1 is empty and there is no one waiting
                pcb->state = "USING DEVICE2";
                IO2.pcb = pcb; // IO1 has chosen thread 
                usleep(T2 * 1000); // Operates on I01 for t1 milisecond
                pcb->device2_io_count += 1;
                pthread_cond_signal(&IO2.cv); // wakes random thread from I01.cv's ready queue
                IO2.pcb = NULL;
            }
            else {
                IO2.count += 1; // Increment IO2 ready queue waiters count 
                enQueue(IO2.queue, *pcb);   // TO DO => Scheduler
                pcb->state = "WAITING DEVICE2";
                pthread_cond_wait(&IO2.cv, &lock); // Thread put in IO1 ready queue(waiting queue)
                deQueue_tid(IO2.queue, pcb->tid);  // TO DO => Scheduler (Optional)
                 
                pcb->state = "USING DEVICE2"; 
                IO2.count -= 1; // Lower IO2 ready queue waiters count 
                IO2.pcb = pcb; // IO1 has chosen thread 
                usleep(T2 * 1000); // Operates on I02 for t2 milisecond
                pcb->device2_io_count += 1;
                pthread_cond_signal(&IO2.cv); // wakes random thread from I01.cv's ready queue
                IO2.pcb = NULL; // I01 is empty now
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

    sem_init(&S, 0, 0);

    // Process generator declaration
    pthread_t generator_tid;

    // CPU initialization
    // cpu-> pcb
    CPU.count = 0;
    CPU.queue = createQueue();
    pthread_cond_init(&CPU.cv, NULL);


    // IO1 initialization
    // IO1-> pcb
    IO1.count = 0;
    IO1.queue = createQueue();
    pthread_cond_init(&IO1.cv, NULL);


    // IO2 initialization
    // IO2-> pcb
    IO2.count = 0;
    IO2.queue = createQueue();
    pthread_cond_init(&IO2.cv, NULL);

    // Process generator creation
    pthread_create(&generator_tid, NULL, process_generator, NULL);


    return 0;
}