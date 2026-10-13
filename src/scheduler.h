#ifndef SCHEDULER_H
#define SCHEDULER_H
#define MAX_ID 32

typedef struct {
    char id[MAX_ID];
    int burst;
    int remaining;
    int arrival;
    int completion;
    int waiting;
    int turnaround;
} Job;

void ejecutar_fcfs(Job jobs[], int cantidad);

void ejecutar_rr(
    Job jobs[],
    int cantidad,
    int quantum
);
#endif
