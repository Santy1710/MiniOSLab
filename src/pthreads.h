#ifndef PTHREADS_H
#define PTHREADS_H

#include <pthread.h>

typedef struct {
    long long inicio;
    long long fin;
    long long suma_parcial;
} RangoHilo;

typedef struct {
    int incrementos;
} SyncArgs;

void *ejecutar_suma(void *arg);
void *incrementar_unsafe(void *arg);
void *incrementar_safe(void *arg);
double medir_tiempo_sec(void);

// Declaracion de la funcion que llamara main.c
int ejecutar_pthreads(int argc, char *argv[]);

#endif // PTHREADS_H
