#ifndef PROCESOS_H
#define PROCESOS_H

#include <sys/types.h>

#define ID_MAX   32
#define TIPO_MAX 16

/* Un trabajo leído de trabajos.txt, por ejemplo: JOB1 SUMA 1 10000000 */
typedef struct {
    char      id[ID_MAX];     /* JOB1 */
    char      tipo[TIPO_MAX]; /* SUMA | PRIMOS */
    long long inicio;
    long long fin;
} Trabajo;

/* Información que el hijo devuelve al padre por el pipe (IPC) */
typedef struct {
    char      id[ID_MAX];
    char      estado[16];     /* OK | ERROR */
    long long resultado;
    double    tiempo;         /* segundos medidos por el worker */
    pid_t     pid;            /* PID del hijo (lo conoce el padre por fork) */
    int       codigo_salida;  /* exit code o 128+señal */
} ResultadoJob;

typedef enum { MODO_SECUENCIAL, MODO_PARALELO } ModoEjecucion;

/*
 * Ejecuta n trabajos. Cada uno corre en un proceso hijo (fork + exec del
 * worker) y devuelve su resultado al padre por un pipe.
 *   worker: ruta del ejecutable worker (NULL = buscar junto al ejecutable actual)
 *   MODO_SECUENCIAL: lanza un hijo, espera, lanza el siguiente.
 *   MODO_PARALELO:   lanza todos los hijos y luego recoge los resultados.
 * Devuelve 0 si todo salió bien, -1 si hubo algún error de sistema.
 */
int ejecutar_trabajos(const Trabajo *trabajos, int n, ResultadoJob *resultados,
                      ModoEjecucion modo, const char *worker);

#endif
