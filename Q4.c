#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <ctype.h>

#define MAX_MESSAGE_LENGTH 100

char global_message[MAX_MESSAGE_LENGTH];
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
    strcpy(global_message, message);
}

void Retirer(char* message) {
    strcpy(message, global_message);
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

    sem_init(&sprod, 0, 1); 
    sem_init(&scons, 0, 0);

    pthread_create(&prodThread, NULL, Production, NULL);
    pthread_create(&consThread, NULL, Consommation, NULL);

    pthread_join(prodThread, NULL);
    pthread_join(consThread, NULL);

    sem_destroy(&sprod);
    sem_destroy(&scons);

    return 0;
}
