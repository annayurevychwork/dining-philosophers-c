#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define COUNT 5
#define THINKING 0
#define WAITING 1
#define EATING 2
#define LEFT_FORK(id) ((id + COUNT - 1) % COUNT)
#define RIGHT_FORK(id) ((id + 1) % COUNT)

typedef struct
{
    int status;
    int id;
    sem_t fork;
} Philosopher;

sem_t table_lock;
Philosopher philosophers[COUNT];

void check_forks(Philosopher * philosopher)
{
    int left = LEFT_FORK(philosopher -> id);
    int right = RIGHT_FORK(philosopher -> id);

    if (philosopher -> status == WAITING && philosophers[left].status != EATING && philosophers[right].status != EATING)
    {
        philosopher -> status = EATING;
        usleep(50000 + rand() % 100000);
        printf("Philosopher %d grabbed forks %d and %d and started eating.\n", philosopher->id + 1, left + 1, philosopher -> id + 1);
        sem_post(&philosopher -> fork);
    }
}

void think(Philosopher * philosopher)
{
    printf("Philosopher %d is lost in thoughts...\n", philosopher -> id + 1);
    usleep(100000 + rand() % 300000);
}

void take_forks(Philosopher * philosopher)
{
    sem_wait(&table_lock);
    philosopher -> status = WAITING;
    printf("Philosopher %d wants to eat and is waiting for forks.\n", philosopher -> id + 1);

    check_forks(philosopher);
    
    sem_post(&table_lock);
    sem_wait(&philosopher -> fork);
}

void release_forks(Philosopher* philosopher)
{
    sem_wait(&table_lock);
    philosopher->status = THINKING;
    int left = LEFT_FORK(philosopher -> id);
    int right = RIGHT_FORK(philosopher -> id);
    printf("Philosopher %d finished eating and put down forks %d and %d.\n", philosopher -> id + 1, left + 1, philosopher -> id + 1);

    check_forks(&philosophers[left]);
    check_forks(&philosophers[right]);

    sem_post(&table_lock);
}

void* philosopher_thread(void * arg)
{
    Philosopher * philosopher = (Philosopher*) arg;
    while (1)
    {
        think(philosopher);
        take_forks(philosopher);
        usleep(50000 + rand() % 200000);
        release_forks(philosopher);
    }
}

int main()
{
    pthread_t threads[COUNT];
    sem_init(&table_lock, 0, 1);

    for (int i = 0; i < COUNT; i++) 
    {
        philosophers[i].id = i;
        philosophers[i].status = THINKING;
        sem_init(&philosophers[i].fork, 0, 0);
    }

    for (int i = 0; i < COUNT; i++)
    {
        pthread_create(&threads[i], NULL, philosopher_thread, &philosophers[i]);
        printf("Philosopher %d arrived at the table.\n", i + 1);
    }

    for (int i = 0; i < COUNT; i++)
    {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
