#include "../include/estacionamento.h"
#include "../include/display.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

/* ------------------------------------------------------------------ */
/*  Variáveis globais                                                   */
/* ------------------------------------------------------------------ */

Vaga vagas[TOTAL_VAGAS];
int  vagasOcupadas = 0;

pthread_mutex_t mutexVagas;
pthread_mutex_t cancela;

sem_t semComuns;
sem_t semPCD;
sem_t eventos;

Fila filaEntrada;
Fila filaSaida;

volatile int encerrar = 0;

/* ------------------------------------------------------------------ */
/*  inicializar_estacionamento                                          */
/* ------------------------------------------------------------------ */

void inicializar_estacionamento()
{
    pthread_mutex_init(&mutexVagas, NULL);
    pthread_mutex_init(&cancela,    NULL);

    sem_init(&semComuns, 0, VAGAS_COMUNS);
    sem_init(&semPCD,    0, VAGAS_PCD);
    sem_init(&eventos,   0, 0);

    fila_init(&filaEntrada);
    fila_init(&filaSaida);

    stats.tempo_min = 9999;
    stats.tempo_max = 0;

    for (int i = 0; i < VAGAS_COMUNS; i++)
    {
        sprintf(vagas[i].nome, "C%d", i + 1);
        vagas[i].ocupada = 0;
        vagas[i].pcd     = 0;
    }

    strcpy(vagas[8].nome, "P1");
    vagas[8].ocupada = 0;
    vagas[8].pcd     = 1;

    strcpy(vagas[9].nome, "P2");
    vagas[9].ocupada = 0;
    vagas[9].pcd     = 1;

    printf(COR_SISTEMA
           "\n  ╔══════════════════════════════════════╗\n"
           "  ║   ESTACIONAMENTO INTELIGENTE v1.0    ║\n"
           "  ║   Multithread com pthreads           ║\n"
           "  ╠══════════════════════════════════════╣\n"
           "  ║  Vagas comuns : %-3d                  ║\n"
           "  ║  Vagas PCD    : %-3d                  ║\n"
           "  ║  Total        : %-3d                  ║\n"
           "  ║  Veículos     : %-3d                  ║\n"
           "  ╚══════════════════════════════════════╝\n\n"
           COR_RESET,
           VAGAS_COMUNS, VAGAS_PCD, TOTAL_VAGAS, TOTAL_CARROS);
}

/* ------------------------------------------------------------------ */
/*  atribuirVaga                                                        */
/* ------------------------------------------------------------------ */

int atribuirVaga(Carro *carro)
{
    pthread_mutex_lock(&mutexVagas);

    if (carro->pcd && carro->deseja_vaga_pcd)
    {
        for (int i = VAGAS_COMUNS; i < TOTAL_VAGAS; i++)
        {
            if (!vagas[i].ocupada)
            {
                vagas[i].ocupada = 1;
                strcpy(carro->vaga, vagas[i].nome);
                vagasOcupadas++;

                /* Estatísticas */
                stats.usos_por_vaga[i]++;
                stats.total_pcd_vaga_reservada++;
                if (vagasOcupadas > stats.pico_vagas)
                    stats.pico_vagas = vagasOcupadas;

                pthread_mutex_unlock(&mutexVagas);
                return 1;
            }
        }
    }

    for (int i = 0; i < VAGAS_COMUNS; i++)
    {
        if (!vagas[i].ocupada)
        {
            vagas[i].ocupada = 1;
            strcpy(carro->vaga, vagas[i].nome);
            vagasOcupadas++;

            /* Estatísticas */
            stats.usos_por_vaga[i]++;
            if (carro->pcd) stats.total_pcd_vaga_comum++;
            if (vagasOcupadas > stats.pico_vagas)
                stats.pico_vagas = vagasOcupadas;

            pthread_mutex_unlock(&mutexVagas);
            return 1;
        }
    }

    pthread_mutex_unlock(&mutexVagas);
    return 0;
}

/* ------------------------------------------------------------------ */
/*  liberarVaga                                                         */
/* ------------------------------------------------------------------ */

void liberarVaga(Carro *carro)
{
    pthread_mutex_lock(&mutexVagas);

    for (int i = 0; i < TOTAL_VAGAS; i++)
    {
        if (strcmp(vagas[i].nome, carro->vaga) == 0)
        {
            vagas[i].ocupada = 0;
            vagasOcupadas--;

            /* Estatísticas de tempo */
            long duracao = (long)(time(NULL) - carro->tempo_entrada);
            stats.tempo_total_estacionado += duracao;
            stats.total_saidas++;
            if (duracao < stats.tempo_min) stats.tempo_min = duracao;
            if (duracao > stats.tempo_max) stats.tempo_max = duracao;

            pthread_mutex_unlock(&mutexVagas);

            if (vagas[i].pcd)
                sem_post(&semPCD);
            else
                sem_post(&semComuns);

            return;
        }
    }

    pthread_mutex_unlock(&mutexVagas);
}

/* ------------------------------------------------------------------ */
/*  threadControlador                                                   */
/* ------------------------------------------------------------------ */

void *threadControlador(void *arg)
{
    (void) arg;

    while (1)
    {
        sem_wait(&eventos);

        if (encerrar)
        {
            int val;
            sem_getvalue(&eventos, &val);
            if (val == 0) break;
        }

        /* --- Prioridade 1: saídas --- */
        pthread_mutex_lock(&filaSaida.mutex);
        int temSaida = filaSaida.tamanho;
        pthread_mutex_unlock(&filaSaida.mutex);

        if (temSaida)
        {
            Carro *carro = (Carro *) fila_pop(&filaSaida);

            pthread_mutex_lock(&cancela);

            printf(COR_CONTROLE "\n[Controlador]" COR_RESET
                   " Autorizando saída do "
                   COR_SAIDA "Carro %d" COR_RESET
                   " (vaga " COR_NEGRITO "%s" COR_RESET ")\n",
                   carro->id, carro->vaga);

            liberarVaga(carro);

            printf(COR_SAIDA "[Carro %02d]" COR_RESET
                   " Saiu do estacionamento.\n",
                   carro->id);

            exibir_barra_vagas(vagasOcupadas, TOTAL_VAGAS);

            pthread_mutex_unlock(&cancela);
            continue;
        }

        /* --- Prioridade 2: entradas --- */
        pthread_mutex_lock(&filaEntrada.mutex);
        int temEntrada = filaEntrada.tamanho;
        pthread_mutex_unlock(&filaEntrada.mutex);

        if (temEntrada)
        {
            Carro *carro = (Carro *) fila_pop(&filaEntrada);

            pthread_mutex_lock(&cancela);

            printf(COR_CONTROLE "\n[Controlador]" COR_RESET
                   " Autorizando entrada do "
                   COR_ENTRADA "Carro %d" COR_RESET "%s\n",
                   carro->id,
                   carro->pcd ? COR_PCD " [PCD]" COR_RESET : "");

            if (atribuirVaga(carro))
            {
                carro->tempo_entrada = time(NULL);
                stats.total_entradas++;
                if (carro->pcd) stats.total_pcd++;

                printf(COR_SISTEMA "[Sistema]" COR_RESET
                       " Carro %d → vaga "
                       COR_NEGRITO "%s" COR_RESET "\n",
                       carro->id, carro->vaga);

                exibir_barra_vagas(vagasOcupadas, TOTAL_VAGAS);
            }
            else
            {
                strcpy(carro->vaga, "ERRO");
                printf(COR_SAIDA "[Carro %d]" COR_RESET
                       " Erro interno: sem vaga disponível.\n",
                       carro->id);
            }

            pthread_mutex_unlock(&cancela);

            pthread_mutex_lock(&carro->mutex_vaga);
            carro->vaga_atribuida = 1;
            pthread_cond_signal(&carro->cond_vaga);
            pthread_mutex_unlock(&carro->mutex_vaga);
        }
    }

    printf(COR_CONTROLE "[Controlador]" COR_RESET " Encerrando.\n");
    return NULL;
}

/* ------------------------------------------------------------------ */
/*  threadCarro                                                         */
/* ------------------------------------------------------------------ */

void *threadCarro(void *arg)
{
    Carro *carro = (Carro *) arg;

    printf(COR_ENTRADA "[Carro %02d]" COR_RESET " Chegou%s\n",
           carro->id,
           carro->pcd ? COR_PCD " [PCD]" COR_RESET : "");

    if (carro->pcd && carro->deseja_vaga_pcd)
    {
        printf(COR_ESPERA "[Carro %02d]" COR_RESET
               " Aguardando vaga " COR_PCD "PCD" COR_RESET "...\n",
               carro->id);
        sem_wait(&semPCD);
    }
    else
    {
        printf(COR_ESPERA "[Carro %02d]" COR_RESET
               " Aguardando vaga comum...\n",
               carro->id);
        sem_wait(&semComuns);
    }

    printf(COR_ENTRADA "[Carro %02d]" COR_RESET " Solicitou entrada\n",
           carro->id);

    fila_push(&filaEntrada, carro);
    sem_post(&eventos);

    pthread_mutex_lock(&carro->mutex_vaga);
    while (!carro->vaga_atribuida)
        pthread_cond_wait(&carro->cond_vaga, &carro->mutex_vaga);
    pthread_mutex_unlock(&carro->mutex_vaga);

    printf(COR_ENTRADA "[Carro %02d]" COR_RESET
           " Estacionado na vaga " COR_NEGRITO "%s" COR_RESET "\n",
           carro->id, carro->vaga);

    sleep(rand() % 10 + 1);

    printf(COR_SAIDA "[Carro %02d]" COR_RESET
           " Solicitou saída da vaga " COR_NEGRITO "%s" COR_RESET "\n",
           carro->id, carro->vaga);

    fila_push(&filaSaida, carro);
    sem_post(&eventos);

    return NULL;
}
