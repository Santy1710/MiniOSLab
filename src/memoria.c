#include <stdio.h>
#include <stdlib.h>
#include "memoria.h"

void memory_test(int mb) {
    size_t bytes = (size_t)mb * 1024 * 1024;

    char *memoria = malloc(bytes);

    if (memoria == NULL) {
        printf("No se pudo reservar memoria.\n");
        return;
    }

    printf("Se reservaron %d MB de memoria.\n", mb);

    for (size_t i = 0; i < bytes; i++) {
        memoria[i] = 1;
    }

    printf("Se escribio en los %zu bytes reservados.\n", bytes);

    printf("Presione ENTER para liberar la memoria...\n");
    getchar();

    free(memoria);
}