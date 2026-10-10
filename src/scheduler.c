#include <stdio.h>
#include "scheduler.h"

void ejecutar_fcfs(Job jobs[], int cantidad)
{
    if (cantidad <= 0){
    printf("No hay trabajos para planificar.\n");
    return;
    }
    
    int tiempo = 0;
    double totalEspera = 0;
    double totalRetorno = 0;

    printf("\n=== FCFS ===\n\n");

    for (int i = 0; i < cantidad; i++){
        jobs[i].waiting = tiempo - jobs[i].arrival;
        if (jobs[i].waiting < 0)
            jobs[i].waiting = 0;

        tiempo += jobs[i].burst;
        jobs[i].completion = tiempo;
        jobs[i].turnaround =
            jobs[i].completion - jobs[i].arrival;
        totalEspera += jobs[i].waiting;
        totalRetorno += jobs[i].turnaround;

        printf(
            "%s | Burst: %d | Espera: %d | Retorno: %d\n",
            jobs[i].id,
            jobs[i].burst,
            jobs[i].waiting,
            jobs[i].turnaround
        );
    }

    printf("\nEspera promedio: %.2f\n", totalEspera / cantidad);
    printf("Retorno promedio: %.2f\n", totalRetorno / cantidad);
}
void ejecutar_rr(Job jobs[], int cantidad, int quantum)
{   
    if (cantidad <= 0){
    printf("Error: no hay trabajos para planificar.\n");
    return;
    }

    if (quantum <= 0){
    printf("Error: el quantum debe ser mayor que 0.\n");
    return;
    }

    int tiempo = 0;
    int terminados = 0;

    double totalEspera = 0;
    double totalRetorno = 0;

    printf("\n=== ROUND ROBIN ===\n");
    printf("Quantum: %d\n\n", quantum);

    for (int i = 0; i < cantidad; i++)
    {
        jobs[i].remaining = jobs[i].burst;
        jobs[i].completion = 0;
        jobs[i].waiting = 0;
        jobs[i].turnaround = 0;
    }

    while (terminados < cantidad) {
        for (int i = 0; i < cantidad; i++) {
            if (jobs[i].remaining <= 0)
                continue;

            printf("Tiempo %d: %s ejecuta ", tiempo, jobs[i].id);

            if (jobs[i].remaining > quantum){
                printf("%d unidades", quantum);
                tiempo += quantum;
                jobs[i].remaining -= quantum;
            }

            else {
                int ejecutado = jobs[i].remaining;
                printf("%d unidades", ejecutado);
                tiempo += ejecutado;
                jobs[i].remaining = 0;
                jobs[i].completion = tiempo;
                jobs[i].turnaround = jobs[i].completion - jobs[i].arrival;
                jobs[i].waiting = jobs[i].turnaround - jobs[i].burst;
                terminados++;
                printf(" -> TERMINA");
            }
            printf(" | restante: %d | tiempo actual: %d\n", jobs[i].remaining, 
                tiempo);
        }
    }

    printf("\n--- RESULTADOS ---\n\n");
    for (int i = 0; i < cantidad; i++){
        totalEspera += jobs[i].waiting;
        totalRetorno += jobs[i].turnaround;
        printf(
            "%s | Burst: %d | Espera: %d | Retorno: %d\n",
            jobs[i].id,
            jobs[i].burst,
            jobs[i].waiting,
            jobs[i].turnaround
        );
    }
    printf("\nEspera promedio: %.2f\n", totalEspera / cantidad);
    printf("Retorno promedio: %.2f\n", totalRetorno / cantidad);
}
