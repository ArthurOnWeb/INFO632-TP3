#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <ctype.h>

#define MAX_MESSAGE_LENGTH 100

char message[MAX_MESSAGE_LENGTH];
sem_t sprod;
sem_t scons;

void Produire(char* message) {
    static int k = 0;
    sprintf(message, "Message : %d", k++);
}

void Consommer(char* message) {
    for(int i = 0; message[i]; i++){
        message[i] = toupper(message[i]);
    }
    printf("%s\n", message);
}

void Deposer(char* message) {
    // In a real-world scenario, this function would add the message to a shared buffer.
    // Here we just copy the message to a global variable for simplicity.
    strcpy(::message, message);
}

void Retirer(char* message) {
    // In a real-world scenario, this function would remove the message from a shared buffer.
    // Here we just copy the global variable to the message for simplicity.
    strcpy(message, ::message);
}

void* Production(void* arg) {
    while (1) {
        char message[MAX_MESSAGE_LENGTH];
        Produire(message);
        sem_wait(&sprod);
        Deposer(message);
        sem_post(&scons);
    }
    return NULL;
}

void* Consommation(void* arg) {
    while (1) {
        char message[MAX_MESSAGE_LENGTH];
        sem_wait(&scons);
        Retirer(message);
        sem_post(&sprod);
        Consommer(message);
    }
    return NULL;
}

int main() {
    pthread_t prodThread, consThread;

    // Initialize the semaphores
    sem_init(&sprod, 0, 1);  // initially the producer is allowed to produce
    sem_init(&scons, 0, 0);  // initially the consumer cannot consume

    // Create the threads
    pthread_create(&prodThread, NULL, Production, NULL);
    pthread_create(&consThread, NULL, Consommation, NULL);

    // Wait for the threads to finish
    pthread_join(prodThread, NULL);
    pthread_join(consThread, NULL);

    // Clean up the semaphores
    sem_destroy(&sprod);
    sem_destroy(&scons);

    return 0;
}
