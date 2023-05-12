#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

/* Global variables */
int x = 0, y = 0;
sem_t sync1, sync2;

/* Thread function */
void *p1(void *arg)
{
    a1();
    /* send signal to the created thread */
    sem_post(&sync1);
    /* wait for signal from thread 2 */
    sem_wait(&sync2);
    a2();
    return NULL;
}

void a1(){
  sleep(1);
  printf("a1() X = %d\n", x);
  x=55;
}

void a2(){
  printf("a2() X = %d\n", x);
  x=77;
}

void *p2(void *arg)
{
    b1();
    /* send signal to thread 1 */
    sem_post(&sync2);
    /* wait for signal from thread 1 */
    sem_wait(&sync1);
    b2();
    return NULL;
}

void b1(){
  printf("b1() Y = %d\n", y);
  y=66;
}

void b2(){
  printf("b2() Y = %d\n", y);
  y=88;
}

int main () {
    pthread_t thread1, thread2;

    /* semaphore sync should be initialized by 0 */
    if (sem_init(&sync1, 0, 0) == -1 || sem_init(&sync2, 0, 0) == -1) {
        perror("Could not initialize semaphore");
        exit(2);
    }

    if (pthread_create(&thread1, NULL, p1, NULL) < 0) {
        perror("Error: thread cannot be created");
        exit(1);
    }

    if (pthread_create(&thread2, NULL, p2, NULL) < 0) {
        perror("Error: thread cannot be created");
        exit(1);
    }

    /* wait for created thread to terminate */
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    /* destroy semaphore sync */
    sem_destroy(&sync1);
    sem_destroy(&sync2);

    return 0;
}
