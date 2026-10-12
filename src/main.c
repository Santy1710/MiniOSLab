#include <stdio.h>
#include <unistd.h>
#include "memoria.h"

#include <stdio.h>
#include <unistd.h>
#include "memoria.h"

int main() {
    printf("MiniOSLab iniciado correctamente.\n");

    memory_test(10);

    printf("Tamaño de página: %ld bytes\n", sysconf(_SC_PAGESIZE));

    getchar();
    return 0;
}