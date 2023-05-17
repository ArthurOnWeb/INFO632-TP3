#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>

#define MAX_MESSAGE_LENGTH 100
#define QUEUE_CAPACITY 100
#define NUM_PRODUCERS 5
#define NUM_CONSUMERS 5

// Structure pour représenter une file d'attente
struct Queue
{
    int front, rear, size;
    unsigned capacity;
    char** array; // Changed from int* to char**
};

sem_t sprod;
sem_t scons;
struct Queue* queue;

// function to create a queue of given capacity.
// It initializes size of queue as 0

struct Queue* createQueue(unsigned capacity)
{
    struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;  // This is important, see the enqueue
    queue->array = (char**) malloc(queue->capacity * sizeof(char*));  // Changed from int* to char**
    return queue;
}

// Queue is full when size becomes equal to the capacity
int isFull(struct Queue* queue)
{  return (queue->size == queue->capacity);  }

// Queue is empty when size is 0
int isEmpty(struct Queue* queue)
{  return (queue->size == 0); }

// Function to add an item to the queue.
// It changes rear and size
void enqueue(struct Queue* queue, char* item)
{
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1)%queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
    printf("%s enqueued to queue\n", item);
}

// Function to remove an item from queue.
// It changes front and size
char* dequeue(struct Queue* queue)
{
    if (isEmpty(queue))
        return NULL;
    char* item = queue->array[queue->front];
    queue->front = (queue->front + 1)%queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

// Function to get front of queue
char* front(struct Queue* queue)
{
    if (isEmpty(queue))
        return NULL;
    return queue->array[queue->front];
}

// Function to get rear of queue
char* rear(struct Queue* queue)
{
    if (isEmpty(queue))
        return NULL;
    return queue->array[queue->rear];
}

void* Produire(void* arg) {
    while (1) {
        char* message = malloc(MAX_MESSAGE_LENGTH * sizeof(char));
        sprintf(message, "Message : %d", *(int*)arg);
        sem_wait(&sprod);
        enqueue(queue, message);  // Changez pour utiliser enqueue au lieu de strcpy
        sem_post(&scons);
        sleep(rand() % 5);  // Pause aléatoire pour simuler un temps de production variable
    }
    return NULL;
}

void* Consommer(void* arg) {
    while (1) {
        sem_wait(&scons);
        char* message = dequeue(queue);  // Changez pour utiliser dequeue au lieu de strcpy
        sem_post(&sprod);
        for(int i = 0; message[i]; i++) {
            message[i] = toupper(message[i]);
        }
        sleep(rand() % 5);  // Pause aléatoire avant l'impression
        printf("%s\n", message);
        free(message);
    }
    return NULL;
}

int main() {
    pthread_t prodThreads[NUM_PRODUCERS], consThreads[NUM_CONSUMERS];
    int threadIds[NUM_PRODUCERS];  // Pour donner un identifiant unique à chaque producteur

    queue = createQueue(QUEUE_CAPACITY);
    sem_init(&sprod, 0, QUEUE_CAPACITY);
    sem_init(&scons, 0, 0);
    srand(time(0));  // Initialiser le générateur de nombres aléatoires

    for (int i = 0; i < NUM_PRODUCERS; i++) {
        threadIds[i] = i;
        pthread_create(&prodThreads[i], NULL, Produire, &threadIds[i]);
    }

    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_create(&consThreads[i], NULL, Consommer, NULL);
    }

    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(prodThreads[i], NULL);
    }

    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(consThreads[i], NULL);
    }

    sem_destroy(&sprod);
    sem_destroy(&scons);

    return 0;
}

