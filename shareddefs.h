#ifndef SHAREDDEFS_H
#define SHAREDDEFS_H

#include <pthread.h>

// The queue, front stores the front node of LL and rear stores the
// last node of LL
struct Queue {
    struct QNode *front, *rear;
};

struct PCB {
    int pid;
    pthread_t tid;
    char* state; // READY, RUNNING, WAITING, TERMINATED, USING DEVICEX
    long long next_cpuburst_len;
    long long rem_cpuburst_len;
    long long num_cpuburst;
    long long time_spend_ready;
    int device1_io_count;
    int device2_io_count;
    double arrival_time;
    long long finish_time;
    long long total_exec_time;

};

struct Device {
    struct PCB* pcb;
    pthread_cond_t cv;
    struct Queue* queue;
    int count;
    int isEmpty;
};

// A linked list (LL) node to store a queue entry
struct QNode {
    struct PCB pcb;
    struct QNode* next;
};

struct QNode* newNode(struct PCB pcb);
struct Queue* createQueue();
void enQueue(struct Queue* q, struct PCB pcb);
struct PCB deQueue(struct Queue* q);
void deQueue_pid(struct Queue* q, int pid);
struct PCB deQueue_min(struct Queue* q);
struct PCB* findMin(struct Queue* q);
void update(struct Queue* q, struct PCB pcb);
void printQ(struct Queue* q);

#endif

