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
    // If queue is empty, return NULL.
    if (q->front == NULL)
        return NULL;

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

void deQueue_tid(struct Queue* q, pthread_t tid){

    struct QNode* current = q->front;
    struct QNode* prev = q->front;

    // If front becomes NULL, then change rear also as NULL
    if (q->front->pcb.tid == tid){
        if(q->front == q->rear)
            q->rear = NULL;
        q->front = q->front->next;   
    }

    while(current != NULL){
        if(current->pcb.tid == tid){
            prev->next = current->next;
            if(prev->next == NULL)
                q->rear = prev;
            free(current); 
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

struct PCB* deQueue_min(struct Queue* q){
    struct PCB* temp = findMin(q);
    deQueue_tid(q,temp->tid);
    return temp;
}

void printQ(struct Queue* q) {
    struct QNode* curr = q->front;

    printf("PRINTING\n");
    while (curr != NULL) {
        printf("%lld\n", curr->pcb.next_cpuburst_len);
        curr = curr->next;
    } 
}


