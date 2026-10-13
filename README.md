# MiniOSLab — Sistemas Operativos (EIF 212)

**Universidad Nacional de Costa Rica (UNA)**  
**Escuela de Informática**  
**Curso:** Sistemas Operativos (EIF 212)  
**Profesor:** Mag. Pablo Sandoval Barrantes  
**Semestre:** II Ciclo, 2026  

---

## 👥 Integrantes del Grupo
* **Santiago Jesús Hernández Chaves** — *(Módulo: Procesos e IPC)*
* **Daniela Lacayo Redondo** — *(Módulo: Pthreads y Sincronización)*
* **Aarón García Fonseca** — *(Módulo: Scheduling y Deadlock)*
* **Santiago David Vega Vásquez** — *(Módulo: Memoria Principal)*

---

## 🖥️ Descripción del Proyecto
**MiniOSLab** es una aplicación de consola desarrollada en C para entornos Linux (Ubuntu Server LTS) que integra en un único producto los conceptos fundamentales de los sistemas operativos:

1. **Detección del Entorno:** Identificación dinámica de vCPUs y tamaño de página del sistema.
2. **Hilos POSIX y Paralelización:** Evaluación de cargas de trabajo concurrentes con $1$, $N$ y $2N$ hilos.
3. **Condición de Carrera y Sincronización:** Demostración de acceso concurrente con y sin exclusión mutua (`pthread_mutex_t`).
4. **Planificación de Trabajos (Scheduler):** Simulación de políticas FCFS y Round Robin (con quantum configurable).
5. **Interbloqueos (Deadlock):** Demostración controlada de espera circular y su solución no bloqueante.
6. **Memoria Principal:** Reserva y acceso dinámico a bloques de memoria observando `VmSize` y `VmRSS`.

---

## 📂 Estructura del Repositorio

### 1. Archivos Principales
| Archivo | Descripción |
| :--- | :--- |
| `Makefile` | Define las instrucciones para compilar el proyecto. |
| `README.md` | Contiene la documentación, las instrucciones y la guía de uso. |
| `run.sh` | Ejecuta las pruebas integradas del proyecto. |

---

### 2. Carpeta `src/` — Código Fuente
| Archivo / Módulo | Función Principal |
| :--- | :--- |
| `main.c` | Punto de entrada del programa y procesamiento de argumentos. |
| `pthreads.c` / `pthreads.h` | Implementación y declaraciones relacionadas con hilos y sincronización. |
| `procesos.c` / `procesos.h` | Gestión de procesos y comunicación entre procesos (IPC). |
| `scheduler.c` / `scheduler.h` | Implementación del planificador mediante FCFS y Round Robin. |
| `deadlock.c` / `deadlock.h` | Pruebas relacionadas con los interbloqueos (deadlocks). |
| `memoria.c` / `memoria.h` | Pruebas relacionadas con la memoria principal. |

---

### 3. Carpetas de Apoyo
* **`data/`**: Almacena los archivos de entrada para pruebas (como `scheduler.txt`).
* **`resultados/`**: Almacena los registros (*logs*) y reportes generados por `run.sh`.

```text
TP1_Grupo04/
├── Makefile
├── README.md
├── run.sh
├── src/
│   ├── main.c
│   ├── pthreads.c / pthreads.h
│   ├── procesos.c / procesos.h
│   ├── scheduler.c / scheduler.h
│   ├── deadlock.c / deadlock.h
│   └── memoria.c / memoria.h
├── data/
│   └── scheduler.txt
└── resultados/
    └── ejecucion_completa.log

```

---

## 🛠️ Requisitos del Sistema

* **SO:** Ubuntu Server LTS minimal (sin GUI).
* **Hostname:** `EIF212-G04`
* **Compilador:** `gcc` con soporte C11 (`-std=c11`).
* **Librerías:** Pthreads (`-pthread`) y Matemáticas (`-lm`).
* **Herramientas de construcción:** `make`.

---

## 🚀 Compilación y Ejecución

### 1. Compilación

Para compilar la aplicación principal `minioslab`:

```bash
make

```

Para limpiar ejecutables y archivos binarios intermedios:

```bash
make clean

```

---

## 📜 Sintaxis y Uso de Comandos

```bash
./minioslab [Opciones]

```

### Módulos y Comandos Disponibles:

1. **Información del Entorno (`--info`):**
```bash
./minioslab --info

```


2. **Hilos POSIX y Sincronización (Pthreads):**
* Modo **Unsafe** (demuestra la condición de carrera sin mutex):
```bash
./minioslab --unsafe

```


* Modo **Safe** (protege la sección crítica con `pthread_mutex_t`):
```bash
./minioslab --safe

```




3. **Planificador de Trabajos (Scheduler):**
* **FCFS:**
```bash
./minioslab data/scheduler.txt --scheduler fcfs

```


* **Round Robin:**
```bash
./minioslab data/scheduler.txt --scheduler rr --quantum 2

```




4. **Interbloqueos (Deadlock):**
```bash
./minioslab --deadlock-test

```


5. **Prueba de Memoria Principal:**
```bash
./minioslab --memory-test

```



---

## 🔄 Ejecución Integrada Automática (`run.sh`)

Para ejecutar todas las pruebas en secuencia y guardar los resultados:

```bash
chmod +x run.sh
./run.sh

