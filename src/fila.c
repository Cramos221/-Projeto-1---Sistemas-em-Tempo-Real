#include "../include/fila.h"

void fila_init(Fila *f)
{
    f->inicio  = 0;
    f->fim     = 0;
    f->tamanho = 0;
    pthread_mutex_init(&f->mutex, NULL);
    sem_init(&f->itens_disponiveis, 0, 0);
}

void fila_push(Fila *f, void *item)
{
    pthread_mutex_lock(&f->mutex);
    f->itens[f->fim] = item;
    f->fim = (f->fim + 1) % TAM_FILA;
    f->tamanho++;
    pthread_mutex_unlock(&f->mutex);

    sem_post(&f->itens_disponiveis);
}

void *fila_pop(Fila *f)
{
    sem_wait(&f->itens_disponiveis);

    pthread_mutex_lock(&f->mutex);
    void *item   = f->itens[f->inicio];
    f->inicio    = (f->inicio + 1) % TAM_FILA;
    f->tamanho--;
    pthread_mutex_unlock(&f->mutex);

    return item;
}
