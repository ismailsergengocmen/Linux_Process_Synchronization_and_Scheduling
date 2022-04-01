#include "systemsim.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

void calculateNewCpuBurst(struct PCB* pcb){
    if(strcmp(burst_dist,"fixed") == 0){
        pcb->next_cpuburst_len = burstlen;
        pcb->rem_cpuburst_len = pcb->next_cpuburst_len;
    }
    else if(strcmp(burst_dist,"uniform") == 0){
        pcb->next_cpuburst_len = (double) rand() / RAND_MAX * (max_burst - min_burst) + min_burst;
        pcb->rem_cpuburst_len = pcb->next_cpuburst_len ;
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
        pcb->rem_cpuburst_len = x;
    }
}

void printInfo(struct PCB* pcb, char* text){
    struct timeval currentTime;
    double curTime;

    gettimeofday(&currentTime, NULL);
    curTime = (currentTime.tv_usec * 0.001 + currentTime.tv_sec * 1000);

    if( OUTMODE == 2 ){
        printf("%.2f %d %s\n", (curTime - simulation_start_time), pcb->pid, pcb->state);
    }

    if( OUTMODE == 3 ){
        printf("%.2f %d %s\n", (curTime - simulation_start_time), pcb->pid, text);
    }
}

static void* process_generator(void* param) {
    pthread_t tids[ALLP];

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

        pthread_mutex_lock(&countLock);
        pcb->pid = total_process_count + 1;
        pthread_mutex_unlock(&countLock);

        pthread_create (&tids[i], NULL, processThread, pcb);

        if(OUTMODE == 3){
            printInfo(pcb,"NEW PROCESS CREATED");
        }

        pthread_mutex_lock(&countLock);
        live_process_count++;
        total_process_count++;
        pthread_mutex_unlock(&countLock);
    }


    if(ALLP > 10){
        while (1) {
            usleep(5000);
            double random = (double)rand() / RAND_MAX;

            if (random <= pg) {

                pthread_mutex_lock(&countLock);
                int local_total_process_count = total_process_count;
                int local_live_process_count = live_process_count;
                pthread_mutex_unlock(&countLock);

                if (local_total_process_count < ALLP && local_live_process_count < MAXP) {
                    struct PCB* pcb = (struct PCB*)malloc(sizeof(struct PCB));
                    
                    pthread_mutex_lock(&countLock);
                    pcb->pid = total_process_count + 1;
                    pthread_create (&tids[total_process_count], NULL, processThread, pcb);
                    pthread_mutex_unlock(&countLock);

                    if(OUTMODE == 3){
                        printInfo(pcb,"NEW PROCESS CREATED");
                    }
                    
                    pthread_mutex_lock(&countLock);
                    live_process_count++;
                    total_process_count++;
                    pthread_mutex_unlock(&countLock);
                } 
                else if (local_total_process_count == ALLP) {  
                    pthread_mutex_lock(&countLock);
                    int local_total_process_count = total_process_count;
                    pthread_mutex_unlock(&countLock);

                    for(int i = 0; i < local_total_process_count; i++){
                        pthread_join(tids[i], NULL);
                    }
                    pthread_exit(0);
                }
            }
        }
    }
    
    pthread_mutex_lock(&countLock);
    int local_total_process_count = total_process_count;
    pthread_mutex_unlock(&countLock);

    // We need to wait for all threads
    for(int i = 0; i < local_total_process_count; i++){
        pthread_join(tids[i], NULL);
    }
    
    pthread_exit(NULL);
    return NULL;    
}

static void* cpu_scheduler(void* param) {
    while(1){
        //LOCK-a
        /**
         * If ALG is not RR and CPU is not empty(which is the case mostly in RR), scheduler should sleep
         * But if it is RR, it should not sleep even if CPU is full
         */

        pthread_mutex_lock(&countLock);
        if (live_process_count == 0 && total_process_count >= ALLP) {
            pthread_exit(0);
        } 
        pthread_mutex_unlock(&countLock);

        pthread_mutex_lock(&CPULock);
        while(AWAKESTATUS == 0 || CPU.count == 0 || CPU.isEmpty == 0 ){
            if(CPU.isEmpty == 0 && strcmp(ALG,"RR") == 0){
                break;
            }

            pthread_mutex_lock(&countLock);
            if (live_process_count == 0 && total_process_count >= ALLP) {
                pthread_exit(0);
            }
            pthread_mutex_unlock(&countLock);

            // ! pthread_mutex_lock(&schedulerLock);
            pthread_cond_wait(&scheduler, &CPULock);
            // ! pthread_mutex_unlock(&schedulerLock);

        }
        pthread_mutex_unlock(&CPULock);

        AWAKESTATUS = 0;
        //UNLOCK-a

        if(strcmp(ALG, "FCFS") == 0){

            pthread_mutex_lock(&CPULock);
            struct PCB temp = deQueue(CPU.queue);
            CPU.count -= 1;
            pthread_mutex_unlock(&CPULock);
  
            if(OUTMODE == 3){
                printInfo(&temp,"PROCESS SELECTED FOR CPU");
            }

            temp.state = "RUNNING";
            printInfo(&temp, temp.state);
            pthread_mutex_lock(&CPULock);
            CPU.isEmpty = 0;
            *CPU.pcb = temp;
            pthread_cond_broadcast(&CPU.cv);
            pthread_mutex_unlock(&CPULock);

        }

        else if(strcmp(ALG, "SJF") == 0){

            pthread_mutex_lock(&CPULock);
            struct PCB temp = deQueue_min(CPU.queue);
            CPU.count -= 1;
            pthread_mutex_unlock(&CPULock);

            if(OUTMODE == 3){
                printInfo(&temp,"PROCESS IS SELECTED FOR CPU");
            }

            temp.state = "RUNNING";
            printInfo(&temp, temp.state);
            pthread_mutex_lock(&CPULock);
            CPU.isEmpty = 0;
            *CPU.pcb = temp;
            pthread_mutex_unlock(&CPULock);

            pthread_mutex_lock(&CPULock);
            pthread_cond_broadcast(&CPU.cv);
            pthread_mutex_unlock(&CPULock);
        }

        else{
            pthread_mutex_lock(&CPULock);
            if(CPU.isEmpty == 0){
                CPU.pcb->state = "READY";
                if(OUTMODE == 3){
                    printInfo(CPU.pcb,"PROCESS EXPIRED ITS TIME QUANTUM");
                    printInfo(CPU.pcb,"PROCESS ADDED TO READY QUEUE");
                }
                enQueue(CPU.queue, *CPU.pcb);
                CPU.count += 1;
                CPU.pcb->rem_cpuburst_len -= atoi(Q);
                CPU.isEmpty = 1;
            }

            struct PCB temp = deQueue(CPU.queue);
            CPU.count -= 1;
            pthread_mutex_unlock(&CPULock);

            if(OUTMODE == 3){
                printInfo(&temp,"PROCESS IS SELECTED FOR CPU");
            }

            temp.state = "RUNNING";

            printInfo(&temp, temp.state);
            pthread_mutex_lock(&CPULock);
            CPU.isEmpty = 0;
            *CPU.pcb = temp;
            pthread_cond_broadcast(&CPU.cv);
            pthread_mutex_unlock(&CPULock);
        }
    }
    pthread_exit(NULL);
    return NULL;
}
             
static void* processThread(void* param){
    // points to heap
    struct PCB* pcb = (struct PCB*) param;

    struct timeval start;
    struct timeval queueEnter;
    struct timeval queueLeave;
    struct timeval terminationTime;
    double ready_queue_wait_time;

    gettimeofday(&start, NULL);

    // Filling information 
    pcb->tid = pthread_self();
    pcb->arrival_time = start.tv_usec * (0.001) + start.tv_sec * (1000) - simulation_start_time;
    pcb->rem_cpuburst_len = 0;
    pcb->num_cpuburst = 0;
    pcb->time_spend_ready = 0;
    pcb->device1_io_count= 0;
    pcb->device2_io_count = 0;
    pcb->finish_time = 0;
    pcb->total_exec_time = 0;
    
    // LIFE OF A THREAD //

    while(1){
        if(strcmp(ALG,"RR") != 0 || pcb->rem_cpuburst_len == 0) 
            calculateNewCpuBurst(pcb); // Calculate next cpu burst
        
        pthread_mutex_lock(&CPULock);
        if(strcmp(ALG,"RR") != 0 || ( strcmp(ALG,"RR") == 0 && CPU.isEmpty == 1 )) {
            CPU.count += 1;
            pcb->state = "READY";
            enQueue(CPU.queue, *pcb); // Added to ready queue
            
            if(OUTMODE == 3){
                printInfo(pcb,"ADDED TO READY QUEUE");
            }
        }    
        pthread_mutex_unlock(&CPULock);
        pthread_mutex_lock(&schedulerLock);
        AWAKESTATUS = 1;
        pthread_cond_signal(&scheduler);
        pthread_mutex_unlock(&schedulerLock);

        pthread_mutex_lock(&CPULock);
        gettimeofday(&queueEnter, NULL);
        while(pcb->tid != CPU.pcb->tid){
            pthread_cond_wait(&CPU.cv, &CPULock); // Add itself to CPU.cv.ready queue and sleep
        }
        gettimeofday(&queueLeave, NULL);
        pthread_mutex_unlock(&CPULock);

        ready_queue_wait_time = (queueLeave.tv_usec * 0.001 + queueLeave.tv_sec * 1000) - (queueEnter.tv_usec * 0.001 + queueEnter.tv_sec * 1000);
        pcb->time_spend_ready += ready_queue_wait_time;

        pthread_mutex_lock(&CPULock);
        update(CPU.queue, *pcb);
        pthread_mutex_unlock(&CPULock);

        // Add RR check and change usleep time accordingly   
        int sleeptime;
        if(strcmp(ALG, "RR") == 0){
            if(pcb->rem_cpuburst_len < atoi(Q)){
                sleeptime = pcb->rem_cpuburst_len;
            }
            else{
                sleeptime = atoi(Q);
            }
        }
        else{
            sleeptime = pcb->next_cpuburst_len;
        }

        usleep(sleeptime * 1000);  // If it wakes, it means it's in CPU so it will usleep

        pthread_mutex_lock(&CPULock);
        pcb->total_exec_time += sleeptime;
        pcb->rem_cpuburst_len -= sleeptime;
        pcb->num_cpuburst += 1;
        update(CPU.queue, *pcb);
        pthread_mutex_unlock(&CPULock);

        if(strcmp(ALG, "RR") != 0){
            pthread_mutex_lock(&CPULock);
            CPU.isEmpty = 1;
            pthread_mutex_unlock(&CPULock);

        }
        pthread_mutex_lock(&schedulerLock);
        AWAKESTATUS = 1;
        pthread_cond_signal(&scheduler);
        pthread_mutex_unlock(&schedulerLock);

        if(pcb->rem_cpuburst_len == 0){
            double chance = (double) rand() / RAND_MAX;

            double minInterval = p0;
            double midInterval = p0 + p1;
            
            if(chance < minInterval){ // Terminate
                gettimeofday(&terminationTime, NULL);
                double endTime = (terminationTime.tv_usec * 0.001 + terminationTime.tv_sec * 1000);
                pcb->finish_time = endTime - simulation_start_time;
                pcb->state = "TERMINATED";
                enQueue(TERMINATED, *pcb); // PCB added to terminated queue   
                if(OUTMODE == 3){
                    printInfo(pcb,pcb->state);
                }

                pthread_mutex_lock(&countLock);
                live_process_count--;
                pthread_mutex_unlock(&countLock);

                pthread_mutex_lock(&schedulerLock);
                pthread_cond_signal(&scheduler);
                pthread_mutex_unlock(&schedulerLock);

                pthread_exit(0); // Thread ended
            }

            else if(minInterval < chance && chance < midInterval) { //IO1
                
                pthread_mutex_lock(&IO1QueueLock);
                enQueue(IO1.queue, *pcb);

                IO1.count += 1; // Increment IO1 ready queue waiters count  

                if(IO1.count - 1 != 0 || IO1.isEmpty == 0){
                    pcb->state = "WAITING DEVICE1";
                    if(OUTMODE == 3){
                        printInfo(pcb,"PROCESS ADDED TO DEVICE 1 QUEUE");
                    }
                    pthread_cond_wait(&IO1.cv, &IO1QueueLock); // Thread put in IO1 ready queue(waiting queue)
                }
                
                deQueue_pid(IO1.queue, pcb->pid);
                pthread_mutex_unlock(&IO1QueueLock);

                pcb->state = "USING DEVICE1"; 

                pthread_mutex_lock(&IO1RunLock);
                IO1.count -= 1; // Lower IO1 ready queue waiters count 
                IO1.isEmpty = 0; //IO1 is full 
                
                IO1.pcb = pcb; // IO1 has chosen thread
                printInfo(pcb, pcb->state);

                usleep(T1 * 1000); // Operates on I01 for t1 milisecond
                pcb->device1_io_count += 1;

                pthread_cond_signal(&IO1.cv); // wakes random thread from I01.cv's ready queue
                IO1.isEmpty = 1; // I01 is empty now
                pthread_mutex_unlock(&IO1RunLock);
                
            }

            else{ // IO2

                pthread_mutex_lock(&IO2QueueLock);
                enQueue(IO2.queue, *pcb);

                IO2.count += 1; // Increment IO2 ready queue waiters count  

                if(IO2.count - 1 != 0 || IO2.isEmpty == 0){
                    pcb->state = "WAITING DEVICE2";
                    if(OUTMODE == 3){
                        printInfo(pcb,"PROCESS ADDED TO DEVICE 2 QUEUE");
                    }
                    pthread_cond_wait(&IO2.cv, &IO2QueueLock); // Thread put in IO2 ready queue(waiting queue)
                }
                
                deQueue_pid(IO2.queue, pcb->pid);
                pthread_mutex_unlock(&IO2QueueLock);
                
                pcb->state = "USING DEVICE2"; 
             
                pthread_mutex_lock(&IO2RunLock);
                IO2.count -= 1; // Lower IO2 ready queue waiters count 
                IO2.isEmpty = 0; //IO2 is full 
                
                IO2.pcb = pcb; // IO2 has chosen thread
                printInfo(pcb, pcb->state);

                usleep(T2 * 1000); // Operates on IO2 for t2 milisecond
                pcb->device2_io_count += 1;
                
                pthread_cond_signal(&IO2.cv); // wakes random thread from IO2.cv's ready queue
                IO2.isEmpty = 1; // IO2 is empty now
                pthread_mutex_unlock(&IO2RunLock);
              
            } 
        }
    }
    return NULL;
}

int main(int argc, char** argv) {
    srand(time(NULL));
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    simulation_start_time = (currentTime.tv_usec * 0.001 + currentTime.tv_sec * 1000);
    ALG = argv[1];

    Q = argv[2];

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

    // Initialize lock
    pthread_mutex_init(&countLock, NULL);
    pthread_mutex_init(&schedulerLock, NULL);
    pthread_mutex_init(&CPULock, NULL);
    pthread_mutex_init(&IO1QueueLock, NULL);
    pthread_mutex_init(&IO1RunLock, NULL);
    pthread_mutex_init(&IO2QueueLock, NULL);
    pthread_mutex_init(&IO2RunLock, NULL);

    // Process generator declaration
    pthread_t generator_tid;

    // Scheduler decleration
    pthread_t scheduler_tid;

    // CPU initialization
    CPU.pcb = malloc(sizeof(struct PCB));
    CPU.isEmpty = 1;
    CPU.count = 0;
    CPU.queue = createQueue();
    pthread_cond_init(&CPU.cv, NULL);

    // IO1 initialization
    IO1.pcb = malloc(sizeof(struct PCB));
    IO1.isEmpty = 1;
    IO1.count = 0;
    IO1.queue = createQueue();
    pthread_cond_init(&IO1.cv, NULL);

    // IO2 initialization
    IO2.pcb = malloc(sizeof(struct PCB));
    IO2.isEmpty = 1;
    IO2.count = 0;
    IO2.queue = createQueue();
    pthread_cond_init(&IO2.cv, NULL);

    TERMINATED = createQueue();

    // Process generator creation
    pthread_create(&generator_tid, NULL, process_generator, NULL);
    pthread_create(&scheduler_tid, NULL, cpu_scheduler, NULL);

    pthread_join(scheduler_tid, NULL);
    pthread_join(generator_tid, NULL);

    
    printQ(TERMINATED);
    return 0;
}