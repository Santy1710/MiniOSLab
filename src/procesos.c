#define _GNU_SOURCE
#include "procesos.h"

#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

/* ---------- utilidades ---------- */

/* Ruta del worker: mismo directorio que el ejecutable actual. */
static int ruta_worker_por_defecto(char *salida, size_t tam)
{
    char exe[PATH_MAX];
    ssize_t n = readlink("/proc/self/exe", exe, sizeof(exe) - 1);
    if (n < 0) {
        snprintf(salida, tam, "./worker");
        return 0;
    }
    exe[n] = '\0';
    snprintf(salida, tam, "%s/worker", dirname(exe));
    return 0;
}

/* ---------- PASO 1: crear hijo, exec del worker ---------- */

/*
 * Crea el pipe, hace fork() y en el hijo ejecuta el worker con exec().
 * El padre se queda con el extremo de LECTURA (fd_lectura) y cierra el de escritura.
 */
static int lanzar_trabajo(const Trabajo *t, const char *worker,
                          pid_t *pid_hijo, int *fd_lectura)
{
    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        return -1;
    }
    /* El extremo de lectura no debe heredarse por los workers (modo paralelo). */
    fcntl(fd[0], F_SETFD, FD_CLOEXEC);

    printf("[PADRE PID=%d] Lanzando %s\n", getpid(), t->id);
    fflush(stdout); /* evita duplicar el buffer de stdout en el hijo */

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        close(fd[0]);
        close(fd[1]);
        return -1;
    }

    if (pid == 0) {
        /* ===== HIJO ===== */
        close(fd[0]); /* el hijo solo escribe */

        char sfd[16], sini[32], sfin[32];
        snprintf(sfd, sizeof(sfd), "%d", fd[1]);
        snprintf(sini, sizeof(sini), "%lld", t->inicio);
        snprintf(sfin, sizeof(sfin), "%lld", t->fin);

        /* El fd del pipe sobrevive a exec(); se lo pasamos al worker por argv. */
        execl(worker, worker, sfd, t->id, t->tipo, sini, sfin, (char *)NULL);

        /* Si llegamos aquí, exec() falló */
        perror("execl");
        _exit(127);
    }

    /* ===== PADRE ===== */
    close(fd[1]); /* el padre solo lee; si no se cierra, read() nunca vería EOF */
    *pid_hijo = pid;
    *fd_lectura = fd[0];
    return 0;
}

/* ---------- PASO 2: leer del pipe y esperar al hijo ---------- */

static void recolectar_resultado(const Trabajo *t, pid_t pid, int fd,
                                 ResultadoJob *r)
{
    char buf[256];
    size_t total = 0;

    memset(r, 0, sizeof(*r));
    snprintf(r->id, sizeof(r->id), "%s", t->id);
    r->pid = pid;

    /* read() hasta EOF (el hijo cerró su extremo al terminar) */
    for (;;) {
        ssize_t n = read(fd, buf + total, sizeof(buf) - 1 - total);
        if (n > 0) {
            total += (size_t)n;
            if (total >= sizeof(buf) - 1) break;
        } else if (n == 0) {
            break;
        } else if (errno != EINTR) {
            perror("read");
            break;
        }
    }
    buf[total] = '\0';
    close(fd);

    /* waitpid(): evita zombies y permite conocer cómo terminó el hijo */
    int status = 0;
    while (waitpid(pid, &status, 0) == -1) {
        if (errno != EINTR) {
            perror("waitpid");
            break;
        }
    }
    if (WIFEXITED(status))
        r->codigo_salida = WEXITSTATUS(status);
    else if (WIFSIGNALED(status))
        r->codigo_salida = 128 + WTERMSIG(status);

    /* Formato del mensaje: id|estado|resultado|tiempo\n */
    char id_rx[ID_MAX];
    if (sscanf(buf, "%31[^|]|%15[^|]|%lld|%lf", id_rx, r->estado,
               &r->resultado, &r->tiempo) != 4 ||
        r->codigo_salida != 0) {
        snprintf(r->estado, sizeof(r->estado), "ERROR");
        r->resultado = -1;
    }

    printf("[PADRE] %s finalizado (PID hijo=%d, estado=%s, resultado=%lld)\n",
           t->id, (int)pid, r->estado, r->resultado);
    fflush(stdout);
}

/* ---------- API pública ---------- */

int ejecutar_trabajos(const Trabajo *trabajos, int n, ResultadoJob *resultados,
                      ModoEjecucion modo, const char *worker)
{
    char ruta[PATH_MAX];
    int errores = 0;

    if (worker == NULL) {
        ruta_worker_por_defecto(ruta, sizeof(ruta));
        worker = ruta;
    }
    if (access(worker, X_OK) != 0) {
        fprintf(stderr, "No se puede ejecutar el worker '%s': %s\n",
                worker, strerror(errno));
        return -1;
    }

    pid_t *pids = calloc((size_t)n, sizeof(pid_t));
    int   *fds  = calloc((size_t)n, sizeof(int));
    if (!pids || !fds) {
        free(pids);
        free(fds);
        return -1;
    }

    if (modo == MODO_SECUENCIAL) {
        for (int i = 0; i < n; i++) {
            if (lanzar_trabajo(&trabajos[i], worker, &pids[i], &fds[i]) != 0) {
                snprintf(resultados[i].id, ID_MAX, "%s", trabajos[i].id);
                snprintf(resultados[i].estado, 16, "ERROR");
                resultados[i].resultado = -1;
                errores++;
                continue;
            }
            recolectar_resultado(&trabajos[i], pids[i], fds[i], &resultados[i]);
        }
    } else {
        /* Primero lanzamos todos (los hijos corren al mismo tiempo)... */
        for (int i = 0; i < n; i++) {
            if (lanzar_trabajo(&trabajos[i], worker, &pids[i], &fds[i]) != 0) {
                pids[i] = -1;
                errores++;
            }
        }
        /* ...y luego recogemos cada resultado y hacemos waitpid. */
        for (int i = 0; i < n; i++) {
            if (pids[i] == -1) {
                snprintf(resultados[i].id, ID_MAX, "%s", trabajos[i].id);
                snprintf(resultados[i].estado, 16, "ERROR");
                resultados[i].resultado = -1;
                continue;
            }
            recolectar_resultado(&trabajos[i], pids[i], fds[i], &resultados[i]);
        }
    }

    free(pids);
    free(fds);
    return errores ? -1 : 0;
}
