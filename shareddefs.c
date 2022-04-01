#include <stdio.h>
#include <stdlib.h>
#include "shareddefs.h"

extern int ALLP;

struct QNode* newNode(struct PCB pcb)
{
    struct QNode* temp = (struct QNode*)malloc(sizeof(struct QNode));
    temp->pcb = pcb;
    temp->next = NULL;
    return temp;
}

struct Queue* createQueue()
{
    struct Queue* q = (struct Queue*)malloc(sizeof(struct Queue));
    q->front = NULL;
    q->rear = NULL;
    return q;
}

void enQueue(struct Queue* q, struct PCB pcb)
{
    struct QNode* temp = newNode(pcb);
    if (q->rear == NULL) {
        q->front = temp;
        q->rear = temp;
        return;
    }

    q->rear->next = temp;
    q->rear = temp;
}

struct PCB deQueue(struct Queue* q)
{
    struct QNode* temp = q->front;
    
    q->front = q->front->next;

    if (q->front == NULL)
        q->rear = NULL;

    struct PCB tempPCB = temp->pcb; 
    free(temp);
    return tempPCB;
}

void deQueue_pid(struct Queue* q, int pid){
    if(q->front == NULL)
        return;

    struct QNode* current = q->front;
    struct QNode* prev = q->front;

    // If front becomes NULL, then change rear also as NULL
    if (q->front->pcb.pid == pid){
        if(q->front == q->rear){ 
            q->rear = NULL;
        }
        struct QNode* temp = q->front;
        q->front = q->front->next;
        free(temp);   
    }

    while(current != NULL){
        if(current->pcb.pid == pid){
            prev->next = current->next;
            if(prev->next == NULL){
                q->rear = prev;
            }
            free(current);
            break; 
        }
        else{
            prev = current;
            current = current->next;
        }
    }

}

struct PCB* findMin(struct Queue* q){
    struct QNode* current = q->front;
    struct PCB* min = (struct PCB*)malloc(sizeof(struct PCB));
    
    if(q->front == NULL){
        return NULL;
    }
    
    else if(q->front == q->rear){
        return &q->front->pcb;
    }

    else{
        *min = q->front->pcb;
        while(current != NULL) {
            if(current->pcb.next_cpuburst_len < min->next_cpuburst_len){
                *min = current->pcb;
            }
            else{
                current = current->next;
            }
        }
        return min;
    }
}

struct PCB deQueue_min(struct Queue* q){
    struct PCB temp = *findMin(q);
    int pid = temp.pid;
    deQueue_pid(q, pid);
    return temp;
}

void update(struct Queue* q, struct PCB pcb){
    struct QNode* current = q->front;

    while(current != NULL){
        if(current->pcb.pid == pcb.pid){
            current->pcb = pcb;
            return;
        }
        current = current->next;
    }
}

void printQ(struct Queue* q) {
    struct QNode* curr;
    printf("\n-----PRINTING RESULT TABLE-----\n");
    printf("pid, arv, finish_time, cpu, waitr, turna, n_bursts, n_d1, n_d2\n");

    for(int i = 1; i <= ALLP; i++){
        curr = q-> front;
        while (curr != NULL) {
            if( curr-> pcb.pid == i){
                int pid = curr->pcb.pid;
                double arv = curr->pcb.arrival_time;
                long long finish_time = curr->pcb.finish_time;
                long long cpu = curr->pcb.total_exec_time;
                long long waitr = curr->pcb.time_spend_ready;
                long long turna = curr->pcb.finish_time - curr->pcb.arrival_time;
                int n_bursts = curr->pcb.num_cpuburst;
                int n_d1 = curr->pcb.device1_io_count;
                int n_d2 = curr->pcb.device2_io_count;             

                printf("%d %.2f %lld %lld %lld %lld %d %d %d\n", pid, arv, finish_time, cpu, waitr, turna, n_bursts, n_d1, n_d2 );
                break;
            }
            curr = curr->next;
        } 
    }
}
