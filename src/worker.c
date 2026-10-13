#define _GNU_SOURCE
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/*
 * Uso (lo invoca el padre con exec):
 *   worker <fd_pipe> <id> <SUMA|PRIMOS> <inicio> <fin>
 * Escribe en el pipe una línea:  id|estado|resultado|tiempo\n
 */

static long long suma_intervalo(long long a, long long b)
{
    long long s = 0;
    for (long long i = a; i <= b; i++) s += i;
    return s;
}

static int es_primo(long long n)
{
    if (n < 2) return 0;
    if (n % 2 == 0) return n == 2;
    for (long long d = 3; d * d <= n; d += 2)
        if (n % d == 0) return 0;
    return 1;
}

static long long contar_primos(long long a, long long b)
{
    long long c = 0;
    for (long long i = a; i <= b; i++) c += es_primo(i);
    return c;
}

/* write() puede escribir menos bytes de los pedidos: se repite hasta terminar */
static int escribir_todo(int fd, const char *buf, size_t len)
{
    size_t hecho = 0;
    while (hecho < len) {
        ssize_t n = write(fd, buf + hecho, len - hecho);
        if (n < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        hecho += (size_t)n;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 6) {
        fprintf(stderr, "Uso: %s <fd> <id> <tipo> <inicio> <fin>\n", argv[0]);
        return 2;
    }
    int fd = atoi(argv[1]);
    const char *id = argv[2], *tipo = argv[3];
    long long ini = atoll(argv[4]), fin = atoll(argv[5]);

    printf("[HIJO PID=%d PPID=%d] Procesando %s\n", getpid(), getppid(), id);
    fflush(stdout);

    /* Pausa opcional (segundos) para poder capturar ps/pstree en la evidencia */
    const char *delay = getenv("MINIOSLAB_DELAY");
    if (delay) sleep((unsigned)atoi(delay));

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    long long resultado = -1;
    const char *estado = "OK";
    if (strcmp(tipo, "SUMA") == 0)
        resultado = suma_intervalo(ini, fin);
    else if (strcmp(tipo, "PRIMOS") == 0)
        resultado = contar_primos(ini, fin);
    else
        estado = "ERROR";

    clock_gettime(CLOCK_MONOTONIC, &t1);
    double seg = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;

    char msg[256];
    int len = snprintf(msg, sizeof(msg), "%s|%s|%lld|%.4f\n", id, estado,
                       resultado, seg);
    if (escribir_todo(fd, msg, (size_t)len) != 0) {
        perror("write");
        return 1;
    }
    close(fd);
    return strcmp(estado, "OK") == 0 ? 0 : 1;
}
