#include <pthread.h>

int T1;
int T2;
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

pthread_cond_t rq;
pthread_cond_t device1;
pthread_cond_t device2;
pthread_cond_t scheduler;

static void* process_generator(void* param);
static void* cpu_scheduler(void* param);
static void* process_thread(void* param);