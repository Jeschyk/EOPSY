#include <stdio.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
 
 
#define PHILOSOPHERS 5   // number of philosophers
#define EATING 10   // eating time of each philosopher (in seconds)
#define THINKING 10 // sleeping time of each philosopher (in seconds)
#define WAITING 3 // initial delay (in seconds)
#define NUM_MEALS 3 // number of meals for all the philosophers;
 
 
int sem_id; 
 
void grab_forks(int left_fork_id)
{

int right_fork_id = (left_fork_id + 1 ) % PHILOSOPHERS;
struct sembuf op[2] = 
{
    {left_fork_id, -1, 0}, // semaphore number, semaphore operation(increased or decreased by this value), operation flags (0-blocking operation, IPC_NOWAIT-nonblocking operation)
    {right_fork_id, -1, 0}
}; 
semop(sem_id, op, 2);  // semid (id of semaphore), sembuf(buffer), nsops(no_of_semaphors_on_which_op_is_performed)

}
 
void put_away_forks(int left_fork_id)
{

int right_fork_id = (left_fork_id + 1 ) % PHILOSOPHERS;
struct sembuf op[2] = 
{
    {left_fork_id, 1, 0}, // semaphore number, semaphore operation(increased or decreased by this value), operation flags (0-blocking operation, IPC_NOWAIT-nonblocking operation)
    {right_fork_id, 1, 0}
}; 
semop(sem_id, op, 2); //semid (id of semaphore), sembuf(buffer), nsops(no_of_semaphors_on_which_op_is_performed)
   
}
 
int philosopher(int id)
{
    printf("\nPhilosopher [%d]: started with pid: %d, comes to the table and waits (%ds)\n", id, getpid(), WAITING);
    sleep(WAITING);
    int counter=0;
 
    while(1)
    {
        if (counter == NUM_MEALS)
        {
            printf("\nPhilosopher [%d]: has eatten all the meals and is full\n", id);
            break;
        }
        counter++;

        printf("\nPhilosopher [%d]: trying to grab [Fork:%d & Fork:%d] and is starving\n", id, id, (id + 1) % PHILOSOPHERS);

        grab_forks(id);

        printf("\nPhilosopher [%d]: grabbed [Fork:%d & Fork:%d] and is eating (%ds)\n", id, id, (id + 1) % PHILOSOPHERS, EATING);
 
        sleep(EATING);
 
        printf("\nPhilosopher [%d]: trying to put away [Fork:%d & Fork:%d] and stopped dining\n", id, id, (id + 1) % PHILOSOPHERS);

        put_away_forks(id);

        printf("\nPhilosopher [%d]: has put away [Fork:%d & Fork:%d] and is thinking (%ds)\n", id, id, (id + 1) % PHILOSOPHERS, THINKING);
 
        sleep(THINKING);
    }
}
 
 
int main()
{
    sem_id = semget(IPC_PRIVATE, PHILOSOPHERS, 0777); //Create new semaphore or get id of previously created (if 0 instead of IPC_PRIVATE).
 
    if(sem_id == -1)
    {
    printf("\nError: Allocation of semaphor failed\n");
    exit(1);
    }
 
    for (int i = 0; i < PHILOSOPHERS; i++) // initializing values for semaphores (all forks put away)
    {
        semctl(sem_id, i, SETVAL, 1); // number of semaphore set, number of semaphore, command, command_parameter. Manages the semaphores
    }

    for (int i = 0; i < PHILOSOPHERS; i++) // initializing philosophers
    {
        pid_t pid = fork();
 
        if (pid == -1)
        {
            printf("\nError, fork did not work\n");
            exit(1);
        }
 
        if (pid == 0)
        {
            return philosopher(i);
        }
    }

    int status;

    while (1) // Call in a loop the wait() function
	{
		status = wait(NULL);
		if (status < 0)
		{
			printf("\nNo more dining Philosophers.\n"); //Printing a message that there are no more dining philosophers
			break;
		}
	}
        if (semctl(sem_id, 0, IPC_RMID, 1) == -1)
        {
            printf("\nError during deallocating semaphores.\n");
            exit(1);
        }
 
    return 0;
}