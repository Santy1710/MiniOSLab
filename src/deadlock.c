#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include "deadlock.h"
#include <time.h>

static pthread_mutex_t recurso1;
static pthread_mutex_t recurso2;

static int intentar_lock_con_timeout(pthread_mutex_t *mutex, int segundos){
    struct timespec limite;
    clock_gettime(CLOCK_REALTIME, &limite);
    limite.tv_sec += segundos;
    return pthread_mutex_timedlock(
        mutex,
        &limite
    );
}

static void *hilo1_deadlock(void *arg){
    (void)arg;
    printf("Thread 1 intentando adquirir R1...\n");
    pthread_mutex_lock(&recurso1);
    printf("Thread 1 adquirio R1\n");
    sleep(1);
    printf("Thread 1 esperando R2...\n");

    int resultado =
        intentar_lock_con_timeout(&recurso2, 2);
    if (resultado == 0) {
        printf("Thread 1 adquirio R2\n");
        pthread_mutex_unlock(&recurso2);
    }
    else if (resultado == ETIMEDOUT) {
        printf(
            "Thread 1: timeout esperando R2\n"
        );
    }

    pthread_mutex_unlock(&recurso1);
    return NULL;
}

static void *hilo2_deadlock(void *arg){
    (void)arg;
    printf("Thread 2 intentando adquirir R2...\n");
    pthread_mutex_lock(&recurso2);
    printf("Thread 2 adquirio R2\n");
    sleep(1);
    printf("Thread 2 esperando R1...\n");
    int resultado =
        intentar_lock_con_timeout(&recurso1, 2);

    if (resultado == 0) {
        printf("Thread 2 adquirio R1\n");
        pthread_mutex_unlock(&recurso1);
    }
    else if (resultado == ETIMEDOUT) {
        printf("Thread 2: timeout esperando R1\n");
    }

    pthread_mutex_unlock(&recurso2);
    return NULL;
}


static void *hilo1_safe(void *arg) {
    (void)arg;
    printf("Thread 1 intentando adquirir R1...\n");
    pthread_mutex_lock(&recurso1);
    printf("Thread 1 adquirio R1\n");
    sleep(1);
    printf("Thread 1 intentando adquirir R2...\n");

    pthread_mutex_lock(&recurso2);
    printf("Thread 1 adquirio R2\n");
    printf("Thread 1 ejecutando seccion con ambos recursos\n");
    pthread_mutex_unlock(&recurso2);
    pthread_mutex_unlock(&recurso1);
    printf("Thread 1 libero R2 y R1\n");
    return NULL;
}


static void *hilo2_safe(void *arg){
    (void)arg;
    printf("Thread 2 intentando adquirir R1...\n");
    pthread_mutex_lock(&recurso1);
    printf("Thread 2 adquirio R1\n");
    sleep(1);
    printf("Thread 2 intentando adquirir R2...\n");
    
    pthread_mutex_lock(&recurso2);
    printf("Thread 2 adquirio R2\n");
    printf("Thread 2 ejecutando seccion con ambos recursos\n");
    pthread_mutex_unlock(&recurso2);
    pthread_mutex_unlock(&recurso1);
    printf("Thread 2 libero R2 y R1\n");
    return NULL;
}


void ejecutar_deadlock_test(void) {
    pthread_t hilo1;
    pthread_t hilo2;
    printf("\n=== PRUEBA DE DEADLOCK ===\n\n");
    pthread_mutex_init(&recurso1, NULL);
    pthread_mutex_init(&recurso2, NULL);
    pthread_create(&hilo1, NULL, hilo1_deadlock, NULL);
    pthread_create(&hilo2, NULL, hilo2_deadlock, NULL);
    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);
    pthread_mutex_destroy(&recurso1);
    pthread_mutex_destroy(&recurso2);
    printf("\nTimeout: posible deadlock.\n");
}

void ejecutar_deadlock_safe(void){
    pthread_t hilo1;
    pthread_t hilo2;
    printf("\n=== SIN DEADLOCK ===\n\n");
    pthread_mutex_init(&recurso1, NULL);
    pthread_mutex_init(&recurso2, NULL);

    pthread_create(&hilo1, NULL, hilo1_safe, NULL);
    pthread_create(&hilo2, NULL, hilo2_safe, NULL);

    pthread_join(hilo1, NULL);
    pthread_join(hilo2, NULL);
    pthread_mutex_destroy(&recurso1);
    pthread_mutex_destroy(&recurso2);

    printf(
        "\nEjecucion finalizada correctamente: "
        "no se produjo deadlock.\n" );
}