#include "proj2.h"

/**
 * Print help message
*/
void print_help(void) {
    fprintf(stderr, "Usage: <num_skiers> <num_stops> <capacity> <wait_time> <travel_time>\n");
}
/**
 * Print all arguments
*/
void print_args(int num_skiers, int num_stops, int capacity, int wait_time, int travel_time){
    printf("Skiers: %d\n", num_skiers);
    printf("Stops: %d\n", num_stops);
    printf("Capacity: %d\n", capacity);
    printf("Wait time: %d\n", wait_time);
    printf("Travel time: %d\n", travel_time);
	printf("------------------------------\n");
}
/**
 * Semaphore init
*/
void init_semaphore(sem_t **semaphore, int initialValue, char *name)
{
	*semaphore = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (*semaphore == MAP_FAILED)
	{
		fprintf(stderr, "Error: shared memory - semaphore: %s\n", name);
		exit(1);
	}

	if (sem_init(*semaphore, 1, initialValue) == -1)
	{
		fprintf(stderr, "Error: semaphore init - semaphore: %s\n", name);
		exit(1);
	}

	// printf("Semaphore %s created\n", name);
}
/**
 * Counter for output
*/
void init_counter(int **counter, int initialValue, char *name)
{
	*counter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if (*counter == MAP_FAILED)
	{
		fprintf(stderr, "Error: shared memory - counter: %s\n", name);
		exit(1);
	}

	**counter = initialValue;

	// printf("Integer %s created with initial value %d\n", name, initialValue);
}
/**
 * Capacity of bus_stop and than of actual skibus
*/
void init_capacity(int **capacity, int size, int initialValue, char *name)
{
    *capacity = mmap(NULL, sizeof(int) * size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (*capacity == MAP_FAILED)
    {
        fprintf(stderr, "Error: shared memory - capacity: %s\n", name);
        exit(1);
    }

    for (int i = 1; i <= size; i++)
    {
        (*capacity)[i] = initialValue;
    }
}
/**
 * Skier process
*/
void skier_process(sem_t *main_sem, sem_t **bus_stop_sems, sem_t *bus_boarding_sem, sem_t *bus_unboarding_sem, int *counter, int *skier_id, int *breakfast_time, int *bus_stop, int **capacity, FILE *file)
{
    // Start
    sem_wait(main_sem);
	fprintf(file, "%d: L %d: started\n", *counter, *skier_id);
    ++(*counter);
    sem_post(main_sem);

    // Breakfast
	srand(getpid());
	int breakfast = rand() % (*breakfast_time + 1);
    usleep(breakfast);

	// Arrived to bus stop
    sem_wait(main_sem);
	fprintf(file, "%d: L %d: arrived to %d\n", *counter, *skier_id, *bus_stop);
    ++(*counter);
    sem_post(main_sem);

    // Wait at bus stop
    sem_wait(bus_stop_sems[*bus_stop]);

	// Boarding
    sem_wait(main_sem);
	fprintf(file, "%d: L %d: boarding\n", *counter, *skier_id);
    ++(*counter);
    sem_post(main_sem);

	// Boarding
	(*capacity)[*bus_stop]--;

    // Signal that skier has finished boarding
    sem_post(bus_boarding_sem);

    // Unboarding
    sem_wait(bus_unboarding_sem);

	// Going to ski
    sem_wait(main_sem);
	fprintf(file, "%d: L %d: going to ski\n", *counter, *skier_id);
    ++(*counter);
    sem_post(main_sem);

    // Signal that skier has finished unboarding
    sem_post(bus_boarding_sem);
}
/**
 * Skibus process
*/
void skibus_process(sem_t *main_sem, sem_t **bus_stop_sems, sem_t *bus_boarding_sem, sem_t *bus_unboarding_sem, int *counter, int *num_stops, int *bus_capacity, int *travel_time, int **capacity, FILE *file)
{	
	// Variable for loop
	bool go_again = true;

	// Start
    sem_wait(main_sem);
	fprintf(file, "%d: BUS: started\n", *counter);
    ++(*counter);
    sem_post(main_sem);

	// Loop
	while (go_again)
	{
		// Define number of passengers
		int num_passengers = 0;
		// Add last stop
		(*num_stops)++;
		for (int current_stop = 1; current_stop <= *num_stops; current_stop++)
		{
			// Time to bus stop
			srand(getpid());
			usleep(rand() % (*travel_time + 1));

			// Check if current stop isn't final
			if (current_stop != *num_stops)
			{
				// Arrived to bus stop
				sem_wait(main_sem);
				fprintf(file, "%d: BUS: arrived to %d\n", *counter, current_stop);
				++(*counter);
				sem_post(main_sem);

				// Boarding
				if (num_passengers < *bus_capacity)
				{
					// Hop on one by one
					int hopons = (*capacity)[current_stop];
					for (int i = 0; i < hopons; i++)
					{
						if (num_passengers == *bus_capacity) 
						{
							break;
						} else if ((*capacity)[current_stop] > 0) // Check if there are skiers at the stop
						{
							// signal skier to board bus
							sem_post(bus_stop_sems[current_stop]);

							// Wait for skier to finish boarding
							sem_wait(bus_boarding_sem);

							// Boarding
							num_passengers++;
						}
					}
					// All skiers from bus stop boarded
					sem_wait(main_sem);
					fprintf(file, "%d: BUS: leaving %d\n", *counter, current_stop);
					++(*counter);
					sem_post(main_sem);

				} else // Full bus capacity
				{
					// Leave bus stop
					sem_wait(main_sem);
					fprintf(file, "%d: BUS: leaving %d\n", *counter, current_stop);
					++(*counter);
					sem_post(main_sem);
					continue;
				}
			} else // It's final stop
			{ 
				// Arrived to final stop
				sem_wait(main_sem);
				fprintf(file, "%d: BUS: arrived to final\n", *counter);
				++(*counter);
				sem_post(main_sem);
				
                // Unboarding
                for (int i = 0; i < num_passengers; i++)
                {
                    // Signal skier to unboard bus
                    sem_post(bus_unboarding_sem);

                    // Wait for skier to finish unboarding
                    sem_wait(bus_boarding_sem);
                }

				// Bus leaves final stop
				fprintf(file, "%d: BUS: leaving final\n", *counter);
				++(*counter);

				// Count remaining skiers
				int remaining_skiers = 0;
				*num_stops = *num_stops - 1;
				for (int i = 1; i <= *num_stops; i++)
				{
					remaining_skiers += (*capacity)[i];
				}

				// Check if there are still skiers
				if (remaining_skiers > 0)
				{
					go_again = true;
				} else
				{
					// Bus finished
					fprintf(file, "%d: BUS: finish\n", *counter);
					go_again = false;
				}
			}
		}
	}
}