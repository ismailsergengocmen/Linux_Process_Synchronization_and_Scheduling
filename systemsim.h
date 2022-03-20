#ifndef SYSTEMSIM_H
#define SYSTEMSIM_H
#include "shareddefs.h"

char* ALG;
int T1;
int T2;
long long Q1;
char* Q2;
char* burst_dist;
long long burstlen ;
long long min_burst;
long long max_burst;
double p0;
double p1;
double p2;
double pg;
int MAXP;
int ALLP;
int OUTMODE;

struct Device CPU;
struct Device IO1;
struct Device IO2;

struct Queue TERMINATED;

pthread_cond_t scheduler;

static void* process_generator(void* param);
static void* cpu_scheduler(void* param);
static void* process_thread(void* param);
void calculateNewCpuBurst(struct PCB* pcb, char* burst_dist, int burstlen, int max_burst, int min_burst);

#endif