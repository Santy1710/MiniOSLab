#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "memoria.h"

#include <stdio.h>
#include <unistd.h>
#include "memoria.h"

#include "scheduler.h"
#include "deadlock.h"

int main(int argc, char *argv[]){
    printf("MiniOSLab iniciado correctamente.\n");
    
    /*Aqui se hace el deadlock*/
    if (argc == 2 && strcmp(argv[1], "--deadlock-test") == 0){
        ejecutar_deadlock_test();
        ejecutar_deadlock_safe();
        return 0;
    }

    /*FCFS*/
    if (argc >= 4 &&
        strcmp(argv[2], "--scheduler") == 0 &&
        strcmp(argv[3], "fcfs") == 0){

        Job jobs[3] = {
            {"JOB1", 5, 0, 0, 0, 0, 0},
            {"JOB2", 3, 0, 0, 0, 0, 0},
            {"JOB3", 8, 0, 0, 0, 0, 0}
        };
        ejecutar_fcfs(jobs, 3);
        return 0;
    }

    /*RR*/
    if (argc >= 6 &&
        strcmp(argv[2], "--scheduler") == 0 &&
        strcmp(argv[3], "rr") == 0 &&
        strcmp(argv[4], "--quantum") == 0) {
        int quantum = atoi(argv[5]);
        if (quantum <= 0) {
            printf("Error: quantum invalido.\n");
            return 1;
        }

        Job jobs[3] = {
            {"JOB1", 5, 0, 0, 0, 0, 0},
            {"JOB2", 3, 0, 0, 0, 0, 0},
            {"JOB3", 8, 0, 0, 0, 0, 0}
        };
        ejecutar_rr(jobs, 3, quantum);
        return 0;
    }

    printf("Uso:\n");
    printf("./minioslab trabajos.txt --scheduler fcfs\n");
    printf("./minioslab trabajos.txt --scheduler rr --quantum N\n");
    printf("./minioslab --deadlock-test\n");

    memory_test(10);

    printf("Tamaño de página: %ld bytes\n", sysconf(_SC_PAGESIZE));

    getchar();
    
    return 0;
}