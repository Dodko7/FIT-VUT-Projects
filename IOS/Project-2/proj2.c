#include "proj2.h"

// MAIN
int main(int argc, char *argv[]) {
    // Check number of arguments
    if (argc != 6) {
        fprintf(stderr, "Invalid number of arguments.\n");
        print_help();
        exit(1);
    }

    // Parse command line arguments
    int num_skiers = atoi(argv[1]);
    int num_stops = atoi(argv[2]);
    int bus_capacity = atoi(argv[3]);
    int breakfast_time = atoi(argv[4]);
    int travel_time = atoi(argv[5]);

    // Check validity of command line arguments
    if (num_skiers < 1 || num_skiers > MAX_SKIERS || num_stops < 1 || num_stops > MAX_STOPS ||
        bus_capacity < MIN_bus_capacity || bus_capacity > MAX_bus_capacity || breakfast_time < 0 || breakfast_time > MAX_breakfast_time ||
        travel_time < 0 || travel_time > MAX_TRAVEL_TIME || argc != 6) {
        fprintf(stderr, "Invalid command line arguments.\n");
        print_help();
        exit(1);
    }

    // Open file
    FILE *file = fopen("proj2.out", "w");
    if (file == NULL) {
        fprintf(stderr, "Error opening file\n");
        exit(1);
    }
    setbuf(file, NULL);

    // Init semaphore
    sem_t *main_sem;
    init_semaphore(&main_sem, 1, "main_sem");
    
    // Init counter
    int *counter;
    init_counter(&counter, 1, "counter");

    // Init capacity
    int **capacity = malloc(num_stops * sizeof(int*)); 
    if (capacity == NULL) {
        fprintf(stderr, "Failed to allocate memory for capacity.\n");
        exit(1);
    }
    init_capacity(capacity, num_stops, 0, "capacity");

    // Create bus boarding semaphore
    sem_t *bus_boarding_sem;
    init_semaphore(&bus_boarding_sem, 0, "bus_boarding_sem");
    // Create bus unboarding semaphore
    sem_t *bus_unboarding_sem;
    init_semaphore(&bus_unboarding_sem, 0, "bus_unboarding_sem");

    // Create an array of semaphores for each bus stop
    sem_t **bus_stop_sems = malloc(sizeof(sem_t *) * (num_stops + 1));
    if (bus_stop_sems == NULL) {
        fprintf(stderr, "Error: could not allocate memory for bus stop semaphores\n");
        return 1;
    }
    // Initialize each semaphore
    for (int i = 1; i <= num_stops; i++) {
        init_semaphore(&bus_stop_sems[i], 0, "bus_stop_semaphore");
    }

    /**
     * Create skier processes
    */
    for (int skier_id = 1; skier_id <= num_skiers; skier_id++) {
        pid_t skier_pid = fork();
        if (skier_pid == 0) {
            // Random bus stop
            srand(getpid());
            int bus_stop = rand() % num_stops + 1;
            (*capacity)[bus_stop]++;
            // Skier process
            skier_process(main_sem, bus_stop_sems, bus_boarding_sem, bus_unboarding_sem, counter, &skier_id, &breakfast_time, &bus_stop, capacity, file);
            exit(0);
        } else if (skier_pid < 0) {
            // Error
            fprintf(stderr, "Error: fork\n");
            // free resources
            exit(1);
        }
    }

    /**
     * Create bus process
    */
    pid_t bus_pid = fork();
    if (bus_pid == 0) {
        // Skibus process
        skibus_process(main_sem, bus_stop_sems, bus_boarding_sem, bus_unboarding_sem, counter, &num_stops, &bus_capacity, &travel_time, capacity, file);
        exit(0);
    } else if (bus_pid < 0) {
        // Error
        fprintf(stderr, "Error: fork\n");
        // free resources
        exit(1);
    }


    // Wait for all skiers to finish
	for (int i = 1; i <= num_skiers + 2; i++)
	{
		wait(NULL);
	}
	while (wait(NULL) > 0);

    // Free memory & destroy semaphores
    for (int i = 1; i <= num_stops; i++) {
        sem_destroy(bus_stop_sems[i]);
        munmap(bus_stop_sems[i], sizeof(sem_t));
        free(capacity[i]);
    }
    free(capacity);
    free(bus_stop_sems);

    // Clean up
    fclose(file);
    sem_destroy(bus_boarding_sem);
    munmap(bus_boarding_sem, sizeof(sem_t));
    sem_destroy(bus_unboarding_sem);
    munmap(bus_unboarding_sem, sizeof(sem_t));
    sem_destroy(main_sem);
    munmap(main_sem, sizeof(sem_t));

    return 0;
}
