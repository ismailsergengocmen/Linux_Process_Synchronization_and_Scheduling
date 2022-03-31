#include <stdio.h>
#include <stdlib.h>
#include "shareddefs.h"

// A utility function to create a new linked list node.
struct QNode* newNode(struct PCB pcb)
{
    struct QNode* temp = (struct QNode*)malloc(sizeof(struct QNode));
    temp->pcb = pcb;
    temp->next = NULL;
    return temp;
}

// A utility function to create an empty queue
struct Queue* createQueue()
{
    struct Queue* q = (struct Queue*)malloc(sizeof(struct Queue));
    q->front = q->rear = NULL;
    return q;
}

// The function to add a key k to q
void enQueue(struct Queue* q, struct PCB pcb)
{
    // Create a new LL node
    struct QNode* temp = newNode(pcb);

    // If queue is empty, then new node is front and rear both
    if (q->rear == NULL) {
        q->front = q->rear = temp;
        return;
    }

    // Add the new node at the end of queue and change rear
    q->rear->next = temp;
    q->rear = temp;
}

// Function to remove a key from given queue q
struct PCB deQueue(struct Queue* q)
{
    // Store previous front and move front one node ahead
    struct QNode* temp = q->front;
    
    q->front = q->front->next;

    // If front becomes NULL, then change rear also as NULL
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
        printf("findMin end: %d\n", min->pid);
        return min;
    }
}

struct PCB deQueue_min(struct Queue* q){
    struct PCB temp = *findMin(q);
    int pid = temp.pid;
    deQueue_pid(q, pid);
    return temp;
}

void printQ(struct Queue* q) {
    struct QNode* curr = q->front;

    printf("\n-----PRINTING-----\n");
    // printf("pid, arv, finish_time, cpu, waitr, turna, n_bursts, n_d1, n_d2\n");
    while (curr != NULL) {
        int pid = curr->pcb.pid;
        // double arv = curr->pcb.arrival_time;
        // long long finish_time = curr->pcb.finish_time;
        // long long cpu = curr->pcb.total_exec_time;
        // long long waitr = curr->pcb.time_spend_ready;
        // long long turna = curr->pcb.finish_time - curr->pcb.arrival_time;
        // int n_bursts = curr->pcb.num_cpuburst;
        // int n_d1 = curr->pcb.device1_io_count;
        // int n_d2 = curr->pcb.device2_io_count;              %f %lld %lld %lld %lld %lld %d %d %d

        printf("****\n%d\n**** \n", pid /*, arv, finish_time, cpu, waitr, turna, n_bursts, n_d1, n_d2*/ );
        curr = curr->next;
    } 
}


