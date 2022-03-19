#include <pthread.h>

struct PCB {
    int pid;
    pthread_t tid;
    int state; // 1- READY, 2- RUNNING, 3-WAITING
    long long next_cpuburst_len;
    long long rem_cpuburt_len;
    long long num_cpuburst;
    long long time_spend_ready;
    int device1_io_count;
    int device2_io_count;
    long long arrival_time;
    long long finish_time;
    long long total_exec_time;
};

struct CPU {
    struct prc;
};

struct IO {
    struct prc;
};
