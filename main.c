#include "include/estacionamento.h"
#include "include/display.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int main()
{
    srand(time(NULL));

    inicializar_estacionamento();

    pthread_t controlador;
    pthread_create(&controlador, NULL, threadControlador, NULL);

    pthread_t carros[TOTAL_CARROS];
    Carro     dados[TOTAL_CARROS];

    for (int i = 0; i < TOTAL_CARROS; i++)
    {
        dados[i].id              = i + 1;
        dados[i].vaga[0]         = '\0';
        dados[i].pcd             = (rand() % 4 == 0); /* ~25% PCD */
        dados[i].deseja_vaga_pcd = rand() % 2;
        dados[i].vaga_atribuida  = 0;
        dados[i].tempo_entrada   = 0;

        pthread_mutex_init(&dados[i].mutex_vaga, NULL);
        pthread_cond_init (&dados[i].cond_vaga,  NULL);

        pthread_create(&carros[i], NULL, threadCarro, &dados[i]);

        usleep(300000); /* 0.3 s entre chegadas */
    }

    for (int i = 0; i < TOTAL_CARROS; i++)
        pthread_join(carros[i], NULL);

    /* Encerramento limpo do controlador */
    encerrar = 1;
    sem_post(&eventos);
    pthread_join(controlador, NULL);

    /* Relatório final */
    exibir_relatorio_final();

    /* Destroy recursos */
    for (int i = 0; i < TOTAL_CARROS; i++)
    {
        pthread_mutex_destroy(&dados[i].mutex_vaga);
        pthread_cond_destroy (&dados[i].cond_vaga);
    }

    pthread_mutex_destroy(&mutexVagas);
    pthread_mutex_destroy(&cancela);
    sem_destroy(&semComuns);
    sem_destroy(&semPCD);
    sem_destroy(&eventos);

    return 0;
}
