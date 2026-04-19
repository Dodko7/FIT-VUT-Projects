#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
/**
 * Define constants
*/
#define MAX_SKIERS 20000
#define MAX_STOPS 10
#define MIN_bus_capacity 10
#define MAX_bus_capacity 100
#define MAX_breakfast_time 10000
#define MAX_TRAVEL_TIME 1000
/**
 * Init semaphores
*/
void print_help(void);
void print_args(int num_skiers, int num_stops, int capacity, int wait_time, int travel_time);
void init_semaphore(sem_t **semaphore, int initialValue, char *name);
void init_counter(int **counter, int initialValue, char *name);
void init_capacity(int **capacity, int size, int initialValue, char *name);
/**
 * Processes
*/
void skier_process(sem_t *main_sem, sem_t **bus_stop_sems, sem_t *bus_boarding_sem, sem_t *bus_unboarding_sem, int *counter, int *skier_id, int *breakfast_time, int *bus_stop, int **capacity, FILE *file);
void skibus_process(sem_t *main_sem, sem_t **bus_stop_sems, sem_t *bus_boarding_sem, sem_t *bus_unboarding_sem, int *counter, int *num_stops, int *bus_capacity, int *travel_time, int **capacity, FILE *file);
