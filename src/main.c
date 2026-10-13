#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "pthreads.h"
#include "memoria.h"
#include "scheduler.h"
#include "deadlock.h"

#define MAX_JOBS 100

int cargar_jobs(const char *archivo, Job jobs[], int *cantidad)
{
    FILE *f = fopen(archivo, "r");

    if (f == NULL) {
        perror("No se pudo abrir el archivo");
        return -1;
    }

    *cantidad = 0;

    while (*cantidad < MAX_JOBS) {
        Job j = {0};

        int leidos = fscanf(f, "%31s %d %d",
                            j.id, &j.burst, &j.arrival);

        if (leidos == EOF)
            break;

        if (leidos != 3 || j.burst <= 0 || j.arrival < 0) {
            fprintf(stderr,
                    "Error en el formato de trabajos.txt\n");
            fclose(f);
            return -1;
        }

        jobs[*cantidad] = j;
        (*cantidad)++;
    }

    if (ferror(f)) {
        perror("Error leyendo los trabajos");
        fclose(f);
        return -1;
    }

    fclose(f);

    if (*cantidad == MAX_JOBS) {
        fprintf(stderr,
                "Se alcanzo el limite de %d trabajos.\n",
                MAX_JOBS);
        return -1;
    }

    if (*cantidad == 0) {
        fprintf(stderr, "El archivo no contiene trabajos.\n");
        return -1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    printf("MiniOSLab iniciado correctamente.\n");

    if (argc == 2 && strcmp(argv[1], "--deadlock-test") == 0) {
        ejecutar_deadlock_test();
        ejecutar_deadlock_safe();
        return 0;
    }

    
if (argc >= 2 && strcmp(argv[1], "--memory-test") == 0) {
    int mb = 10;

    if (argc == 3) {
        char *fin;
        long valor = strtol(argv[2], &fin, 10);

        if (*argv[2] == '\0' || *fin != '\0' ||
            valor <= 0 || valor > 1024) {
            printf("Uso: ./minioslab --memory-test [MB]\n");
            printf("El tamaño debe estar entre 1 y 1024 MB.\n");
            return 1;
        }

        mb = (int)valor;
    } else if (argc != 2) {
        printf("Uso: ./minioslab --memory-test [MB]\n");
        return 1;
    }

    memory_test(mb);
    return 0;
}


    if (argc == 2 &&
    (strcmp(argv[1], "--unsafe") == 0 ||
     strcmp(argv[1], "--safe") == 0)) {
    return ejecutar_pthreads(argc, argv);
}

    if (argc >= 4 &&
        strcmp(argv[2], "--scheduler") == 0) {

        Job jobs[MAX_JOBS];
        int cantidad = 0;

        if (cargar_jobs(argv[1], jobs, &cantidad) != 0)
            return 1;

        if (strcmp(argv[3], "fcfs") == 0 && argc == 4) {
            ejecutar_fcfs(jobs, cantidad);
            return 0;
        }

        if (strcmp(argv[3], "rr") == 0 &&
            argc == 6 &&
            strcmp(argv[4], "--quantum") == 0) {

            char *fin;
            long quantum = strtol(argv[5], &fin, 10);

            if (*argv[5] == '\0' || *fin != '\0' ||
                quantum <= 0 || quantum > 2147483647L) {
                fprintf(stderr, "Error: quantum invalido.\n");
                return 1;
            }

            ejecutar_rr(jobs, cantidad, (int)quantum);
            return 0;
        }

        fprintf(stderr, "Argumentos de scheduler invalidos.\n");
        return 1;
    }

    printf("Uso:\n");
    printf("./minioslab data/scheduler.txt --scheduler fcfs\n");
    printf("./minioslab data/scheduler.txt --scheduler rr --quantum N\n");
    printf("./minioslab --unsafe\n");
    printf("./minioslab --safe\n");
    printf("./minioslab --deadlock-test\n");
    printf("./minioslab --memory-test\n");

    return 0;
}