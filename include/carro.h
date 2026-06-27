#ifndef CARRO_H
#define CARRO_H

#include <pthread.h>
#include <time.h>

typedef struct
{
    int id;
    int pcd;
    int deseja_vaga_pcd;
    char vaga[10];

    /* Tempo para estatísticas */
    time_t tempo_entrada;

    /* Sincronização: controlador sinaliza quando vaga foi atribuída */
    pthread_mutex_t mutex_vaga;
    pthread_cond_t  cond_vaga;
    int vaga_atribuida; /* 0 = aguardando, 1 = atribuída */
} Carro;

#endif
