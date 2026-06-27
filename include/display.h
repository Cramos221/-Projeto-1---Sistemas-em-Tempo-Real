#ifndef DISPLAY_H
#define DISPLAY_H

#include <time.h>

/* ------------------------------------------------------------------ */
/*  Códigos de cor ANSI                                                 */
/* ------------------------------------------------------------------ */

#define COR_RESET      "\033[0m"
#define COR_NEGRITO    "\033[1m"

/* Elementos do sistema */
#define COR_SISTEMA    "\033[1;36m"   /* ciano  negrito  — mensagens do sistema   */
#define COR_CONTROLE   "\033[1;33m"   /* amarelo negrito — controlador            */
#define COR_ENTRADA    "\033[1;32m"   /* verde  negrito  — entrada de carro       */
#define COR_SAIDA      "\033[1;31m"   /* vermelho negrito— saída de carro         */
#define COR_ESPERA     "\033[0;37m"   /* cinza          — carro aguardando        */
#define COR_PCD        "\033[1;35m"   /* magenta negrito — indicador PCD          */
#define COR_VAGA       "\033[0;32m"   /* verde          — vaga comum              */
#define COR_VAGA_PCD   "\033[0;35m"   /* magenta        — vaga PCD               */
#define COR_BARRA      "\033[0;34m"   /* azul           — barra de ocupação       */
#define COR_RELATORIO  "\033[1;37m"   /* branco negrito — relatório final         */
#define COR_TITULO     "\033[1;34m"   /* azul negrito   — títulos                 */

/* ------------------------------------------------------------------ */
/*  Estrutura de estatísticas                                           */
/* ------------------------------------------------------------------ */

typedef struct
{
    int  total_entradas;
    int  total_saidas;
    int  total_pcd;
    int  total_pcd_vaga_reservada;
    int  total_pcd_vaga_comum;
    int  pico_vagas;              /* máximo de vagas ocupadas ao mesmo tempo */
    long tempo_total_estacionado; /* soma dos tempos em segundos             */
    long tempo_min;               /* menor tempo individual                  */
    long tempo_max;               /* maior tempo individual                  */

    /* Por vaga: quantas vezes foi usada */
    int usos_por_vaga[10];
} Estatisticas;

extern Estatisticas stats;

/* ------------------------------------------------------------------ */
/*  Protótipos                                                          */
/* ------------------------------------------------------------------ */

void exibir_barra_vagas(int ocupadas, int total);
void exibir_relatorio_final();

#endif
