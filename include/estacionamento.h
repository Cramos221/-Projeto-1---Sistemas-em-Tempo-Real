#ifndef ESTACIONAMENTO_H
#define ESTACIONAMENTO_H

#include <pthread.h>
#include <semaphore.h>

#include "carro.h"
#include "vaga.h"
#include "fila.h"

#define VAGAS_COMUNS  8
#define VAGAS_PCD     2
#define TOTAL_VAGAS   10
#define TOTAL_CARROS  20

/* Recursos compartilhados */
extern Vaga vagas[TOTAL_VAGAS];
extern int  vagasOcupadas;

extern pthread_mutex_t mutexVagas;
extern pthread_mutex_t cancela;

/*
 * semComuns : conta vagas comuns disponíveis (init = VAGAS_COMUNS)
 * semPCD    : conta vagas PCD   disponíveis (init = VAGAS_PCD)
 * eventos   : notifica o controlador de nova entrada ou saída
 */
extern sem_t semComuns;
extern sem_t semPCD;
extern sem_t eventos;

extern Fila filaEntrada;
extern Fila filaSaida;

/* Flag de encerramento: main seta 1 após todos os carros terminarem */
extern volatile int encerrar;

/* API */
void  inicializar_estacionamento();
int   atribuirVaga (Carro *carro);
void  liberarVaga  (Carro *carro);
void *threadCarro       (void *arg);
void *threadControlador (void *arg);

#endif
