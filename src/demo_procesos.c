#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "procesos.h"

#define MAX_TRABAJOS 64

/* Lector simple de trabajos.txt (líneas como: JOB1 SUMA 1 10000000).
 * Si otro integrante ya tiene el lector (Hito 3), se puede reemplazar. */
static int cargar_trabajos(const char *ruta, Trabajo *arr, int max)
{
    FILE *f = fopen(ruta, "r");
    if (!f) {
        perror(ruta);
        return -1;
    }
    char linea[128];
    int n = 0;
    while (n < max && fgets(linea, sizeof(linea), f)) {
        Trabajo t;
        if (sscanf(linea, "%31s %15s %lld %lld", t.id, t.tipo, &t.inicio,
                   &t.fin) == 4)
            arr[n++] = t;
    }
    fclose(f);
    return n;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Uso: %s trabajos.txt [--seq|--par]\n", argv[0]);
        return 1;
    }
    ModoEjecucion modo = MODO_SECUENCIAL;
    if (argc > 2 && strcmp(argv[2], "--par") == 0) modo = MODO_PARALELO;

    Trabajo trabajos[MAX_TRABAJOS];
    ResultadoJob res[MAX_TRABAJOS];
    int n = cargar_trabajos(argv[1], trabajos, MAX_TRABAJOS);
    if (n <= 0) {
        fprintf(stderr, "No se cargaron trabajos\n");
        return 1;
    }

    printf("=== Demo procesos/IPC (PID padre=%d, modo=%s) ===\n", getpid(),
           modo == MODO_PARALELO ? "paralelo" : "secuencial");
    int rc = ejecutar_trabajos(trabajos, n, res, modo, NULL);

    printf("\n%-8s %-8s %-8s %-16s %-10s\n", "Trabajo", "PID", "Estado",
           "Resultado", "Tiempo(s)");
    for (int i = 0; i < n; i++)
        printf("%-8s %-8d %-8s %-16lld %-10.4f\n", res[i].id, (int)res[i].pid,
               res[i].estado, res[i].resultado, res[i].tiempo);
    return rc == 0 ? 0 : 1;
}
