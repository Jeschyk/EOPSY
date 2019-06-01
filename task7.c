#include <pthread.h>
#include <stdio.h>
#include <unistd.h>


#define N	5
#define LEFT	( i + N - 1 ) % N
#define RIGHT	( i + 1 ) % N

#define THINKING 0
#define HUNGRY 1
#define EATING 2

#define NUM_MEALS 2
#define THINKING_TIME 5
#define EATING_TIME 5
#define WAITING 5
#define up     pthread_mutex_unlock
#define down   pthread_mutex_lock

pthread_mutex_t	m; 		//initialized to 1
int	state[N];	//initiated to THINKING's
pthread_mutex_t	s[N];	//	initialized to 0's
pthread_t philosophers[N];

void test( int i )
{
	if( state[i] == HUNGRY
		&& state[LEFT] != EATING
		&& state[RIGHT] != EATING )
	{
		state[i] = EATING;
		up( &s[i] );
	}
}

void grab_forks( int i )
{
	down( &m );
		state[i] = HUNGRY;
		test( i );
	up( &m );
	down( &s[i] );
}

void put_away_forks( int i )
{
	down( &m );
		state[i] = THINKING;
		test( LEFT );
		test( RIGHT );
	up( &m );
}


void *philosopher(void *i)
{
    int id = (long) i;
    printf("Philosopher [%d] comes to the table and waits.\n", id);
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

        printf("\nPhilosopher [%d]: trying to grab [Fork:%d & Fork:%d] and is hungry\n", id, id, (id + 1) % N);

        grab_forks(id);

        printf("\nPhilosopher [%d]: grabbed [Fork:%d & Fork:%d] and is eating (%ds)\n", id, id, (id + 1) % N, EATING_TIME);
 
        sleep(EATING_TIME);
 
        printf("\nPhilosopher [%d]: trying to put away [Fork:%d & Fork:%d] and stopped eating\n", id, id, (id + 1) % N);

        put_away_forks(id);

        printf("\nPhilosopher [%d]: has put away [Fork:%d & Fork:%d] and is thinking (%ds)\n", id, id, (id + 1) % N, THINKING_TIME);
 
        sleep(THINKING_TIME);
    }


}

int main ()
{
    int i;
    pthread_mutex_init(&m, NULL);
    for (i=0; i<N; ++i)
    {
        state[i]=THINKING;
        pthread_mutex_init(&s[i], NULL);
        down(&s[i]);
        if(pthread_create(&philosophers[i], NULL, (void*)philosopher, (void*)(long)i) != 0)
        {
            printf("Error, cannot create a new philosopher\n");
            return 1;
        }
    }
    for (i=0; i<N; ++i)
    {
        pthread_join(philosophers[i], NULL);
    }
    pthread_mutex_destroy(&m);

    for (i=0; i<N; ++i)
    {
        pthread_mutex_destroy(&s[i]);
    }
    printf("\nNo more dining Philosophers.\n"); //Printing a message that there are no more dining philosophers
    
    pthread_exit(NULL);
    return 0;
    
}