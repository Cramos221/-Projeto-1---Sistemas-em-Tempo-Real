#ifndef FILA_H
#define FILA_H

#include <pthread.h>
#include <semaphore.h>

#define TAM_FILA 100

typedef struct
{
    void *itens[TAM_FILA];

    int inicio;

    int fim;

    int tamanho;

    pthread_mutex_t mutex;

    sem_t itens_disponiveis;

} Fila;

void fila_init(Fila *f);

void fila_push(Fila *f, void *item);

void *fila_pop(Fila *f);

#endif
