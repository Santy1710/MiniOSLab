#include "pthreads.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

long long contador_global = 0;
pthread_mutex_t mutex_contador;

void *ejecutar_suma(void *arg) {
    RangoHilo *rango = (RangoHilo *)arg;
    rango->suma_parcial = 0;
    for (long long i = rango->inicio; i <= rango->fin; i++) {
        rango->suma_parcial += i;
    }
    return NULL;
}

void *incrementar_unsafe(void *arg) {
    SyncArgs *args = (SyncArgs *)arg;
    for (int i = 0; i < args->incrementos; i++) {
        contador_global++;
    }
    return NULL;
}

void *incrementar_safe(void *arg) {
    SyncArgs *args = (SyncArgs *)arg;
    for (int i = 0; i < args->incrementos; i++) {
        pthread_mutex_lock(&mutex_contador);
        contador_global++;
        pthread_mutex_unlock(&mutex_contador);
    }
    return NULL;
}

double medir_tiempo_sec(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec * 1e-9;
}

int ejecutar_pthreads(int argc, char *argv[]) {
    int n_cpus = sysconf(_SC_NPROCESSORS_ONLN);
    if (n_cpus <= 0) n_cpus = 1;

    if (argc > 1 && (strcmp(argv[1], "--unsafe") == 0 || strcmp(argv[1], "--safe") == 0)) {
        int es_safe = (strcmp(argv[1], "--safe") == 0);
        int num_hilos = 8;
        int incrementos = 1000000;
        long long esperado = (long long)num_hilos * incrementos;

        pthread_t hilos[num_hilos];
        SyncArgs args = { .incrementos = incrementos };

        contador_global = 0;
        pthread_mutex_init(&mutex_contador, NULL);

        for (int i = 0; i < num_hilos; i++) {
            if (es_safe) {
                pthread_create(&hilos[i], NULL, incrementar_safe, &args);
            } else {
                pthread_create(&hilos[i], NULL, incrementar_unsafe, &args);
            }
        }

        for (int i = 0; i < num_hilos; i++) {
            pthread_join(hilos[i], NULL);
        }

        pthread_mutex_destroy(&mutex_contador);

        printf("Modo: %s\n", es_safe ? "SAFE (Mutex)" : "UNSAFE (Sin Mutex)");
        printf("Valor esperado: %lld\n", esperado);
        printf("Valor obtenido: %lld\n", contador_global);
        return 0;
    }

    printf("=== PRUEBA DE CARGA PARALELA (Pthreads) ===\n");
    printf("CPUs detectadas (N): %d\n\n", n_cpus);

    int configuraciones[] = {1, n_cpus, 2 * n_cpus};
    long long limite = 500000000LL;

    for (int c = 0; c < 3; c++) {
        int num_hilos = configuraciones[c];
        pthread_t *hilos = malloc(num_hilos * sizeof(pthread_t));
        RangoHilo *rangos = malloc(num_hilos * sizeof(RangoHilo));

        long long bloque = limite / num_hilos;
        long long suma_total = 0;

        double t_inicio = medir_tiempo_sec();

        for (int i = 0; i < num_hilos; i++) {
            rangos[i].inicio = i * bloque + 1;
            rangos[i].fin = (i == num_hilos - 1) ? limite : (i + 1) * bloque;
            pthread_create(&hilos[i], NULL, ejecutar_suma, &rangos[i]);
        }

        for (int i = 0; i < num_hilos; i++) {
            pthread_join(hilos[i], NULL);
            suma_total += rangos[i].suma_parcial;
        }

        double t_fin = medir_tiempo_sec();

        printf("Hilos: %2d | Tiempo: %.4f s | Resultado: %lld\n",
               num_hilos, (t_fin - t_inicio), suma_total);

        free(hilos);
        free(rangos);
    }

    return 0;
}
