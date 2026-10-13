
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "memoria.h"

static void mostrar_uso_memoria(const char *etapa) {
    FILE *archivo = fopen("/proc/self/status", "r");

    if (archivo == NULL) {
        printf("No se pudo consultar el uso de memoria.\n");
        return;
    }

    char linea[256];

    printf("\n--- %s ---\n", etapa);

    while (fgets(linea, sizeof(linea), archivo) != NULL) {
        if (strncmp(linea, "VmSize:", 7) == 0 ||
            strncmp(linea, "VmRSS:", 6) == 0) {
            printf("%s", linea);
        }
    }

    fclose(archivo);
}

void memory_test(int mb) {
    if (mb <= 0) {
        printf("El tamaño debe ser mayor que cero.\n");
        return;
    }

    size_t bytes = (size_t)mb * 1024 * 1024;
    long tamano_pagina = sysconf(_SC_PAGESIZE);

    if (tamano_pagina <= 0) {
        printf("No se pudo obtener el tamaño de página.\n");
        return;
    }

    size_t paginas = bytes / (size_t)tamano_pagina;
    size_t resto = bytes % (size_t)tamano_pagina;

    printf("\n===== PRUEBA DE MEMORIA =====\n");
    printf("Memoria solicitada: %d MB\n", mb);
    printf("Bytes solicitados: %zu\n", bytes);
    printf("Tamaño de página: %ld bytes\n", tamano_pagina);
    printf("Páginas completas: %zu\n", paginas);
    printf("Bytes restantes: %zu\n", resto);
    printf("Páginas necesarias: %zu\n",
           paginas + (resto > 0 ? 1 : 0));

    mostrar_uso_memoria("Antes de reservar");

    char *memoria = malloc(bytes);

    if (memoria == NULL) {
        printf("No se pudo reservar memoria.\n");
        return;
    }

    mostrar_uso_memoria("Después de malloc");

    for (size_t i = 0; i < bytes; i += (size_t)tamano_pagina) {
        memoria[i] = 1;
    }

    memoria[bytes - 1] = 1;

    mostrar_uso_memoria("Después de acceder a las páginas");

    printf("\nSe accedió a la memoria reservada.\n");

    free(memoria);
    memoria = NULL;

    mostrar_uso_memoria("Después de free");

    printf("\n===== FIN DE LA PRUEBA =====\n");
}
