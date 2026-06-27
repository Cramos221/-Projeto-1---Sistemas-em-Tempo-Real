#include "../include/display.h"
#include "../include/vaga.h"

#include <stdio.h>
#include <string.h>

/* Declarações externas — evita include circular com estacionamento.h */
extern Vaga vagas[];
extern int  vagasOcupadas;

#ifndef VAGAS_COMUNS
#define VAGAS_COMUNS  8
#define VAGAS_PCD     2
#define TOTAL_VAGAS   10
#endif

/* Estatísticas globais */
Estatisticas stats = {0};

/* ------------------------------------------------------------------ */
/*  Helpers                                                             */
/* ------------------------------------------------------------------ */

/* Largura interna da caixa (caracteres visíveis entre ║ e ║) = 46 */
#define W 46

/* Imprime W traços '═' */
static void borda_h()   { for(int i=0;i<W;i++) printf("═"); }

/* Linha "  Label          : valor_int" */
static void linha_int(const char *label, int val)
{
    char buf[64];
    snprintf(buf, sizeof(buf), "  %-22s: %d", label, val);
    int vis = (int)strlen(buf);
    int pad = W - vis;
    if (pad < 0) pad = 0;

    printf(COR_BARRA "  ║" COR_RESET "  %-22s: " COR_NEGRITO "%d" COR_RESET,
           label, val);
    for (int i = 0; i < pad; i++) printf(" ");
    printf(COR_BARRA "║\n" COR_RESET);
}

/* Linha "  Label          : valor_long s" */
static void linha_long(const char *label, long val)
{
    char buf[64];
    snprintf(buf, sizeof(buf), "  %-22s: %lds", label, val);
    int vis = (int)strlen(buf);
    int pad = W - vis;
    if (pad < 0) pad = 0;

    printf(COR_BARRA "  ║" COR_RESET "  %-22s: " COR_NEGRITO "%lds" COR_RESET,
           label, val);
    for (int i = 0; i < pad; i++) printf(" ");
    printf(COR_BARRA "║\n" COR_RESET);
}

/* ------------------------------------------------------------------ */
/*  exibir_barra_vagas                                                  */
/* ------------------------------------------------------------------ */

void exibir_barra_vagas(int ocupadas, int total)
{
    int livres = total - ocupadas;
    int pct    = (ocupadas * 100) / total;

    printf(COR_BARRA "\n  ╔"); borda_h(); printf("╗\n" COR_RESET);

    /* Título centralizado */
    {
        const char *titulo = "  MAPA DE VAGAS  ";
        int vis = (int)strlen(titulo);
        int pad = W - vis;
        int lpad = pad / 2;
        int rpad = pad - lpad;
        printf(COR_BARRA "  ║" COR_RESET COR_TITULO);
        for (int i=0;i<lpad;i++) printf(" ");
        printf("%s", titulo);
        for (int i=0;i<rpad;i++) printf(" ");
        printf(COR_BARRA "║\n" COR_RESET);
    }

    printf(COR_BARRA "  ╠"); borda_h(); printf("╣\n" COR_RESET);

    /* Vagas comuns: [C1] [C2] ... [C8] */
    printf(COR_BARRA "  ║" COR_RESET " ");
    for (int i = 0; i < VAGAS_COMUNS; i++)
    {
        if (vagas[i].ocupada)
            printf(COR_SAIDA "[%s]" COR_RESET " ", vagas[i].nome);
        else
            printf(COR_VAGA  "[%s]" COR_RESET " ", vagas[i].nome);
    }
    /* vis = 1 + 8*(4+1) = 41, pad = 46-41 = 5 */
    printf("     " COR_BARRA "║\n" COR_RESET);

    /* Vagas PCD:
     * vis = 1(' ') + 5('PCD: ') + (VAGAS_PCD * 5)('[Px] ') = 6 + 10 = 16
     * pad = W - 16 = 30 */
    {
        int vis_pcd = 1 + 5 + (TOTAL_VAGAS - VAGAS_COMUNS) * 5;
        int pad_pcd = W - vis_pcd;
        if (pad_pcd < 0) pad_pcd = 0;
        printf(COR_BARRA "  ║" COR_RESET " " COR_PCD "PCD:" COR_RESET " ");
        for (int i = VAGAS_COMUNS; i < TOTAL_VAGAS; i++)
        {
            if (vagas[i].ocupada)
                printf(COR_SAIDA    "[%s]" COR_RESET " ", vagas[i].nome);
            else
                printf(COR_VAGA_PCD "[%s]" COR_RESET " ", vagas[i].nome);
        }
        for (int i = 0; i < pad_pcd; i++) printf(" ");
        printf(COR_BARRA "║\n" COR_RESET);
    }

    printf(COR_BARRA "  ╠"); borda_h(); printf("╣\n" COR_RESET);

    /* Barra de progresso: imprime tudo numa linha só, sem ANSI por caractere.
     * W = 46 colunas exatas usando '|' cheio e '.' vazio. */
    {
        int bar_n     = W;
        int bar_cheio = (ocupadas * bar_n) / total;
        const char *cor_bloco = pct >= 90 ? COR_SAIDA
                              : pct >= 60 ? "\033[1;33m"
                                          : COR_ENTRADA;

        char barra[W + 1];
        for (int i = 0; i < bar_n; i++)
            barra[i] = (i < bar_cheio) ? '|' : '.';
        barra[bar_n] = '\0';

        /* Imprime cheios e vazios em dois blocos de cor */
        printf(COR_BARRA "  ║" COR_RESET);
        if (bar_cheio > 0)
            printf("%s%.*s" COR_RESET, cor_bloco, bar_cheio, barra);
        if (bar_cheio < bar_n)
            printf(COR_ESPERA "%s" COR_RESET, barra + bar_cheio);
        printf(COR_BARRA "║\n" COR_RESET);
    }

    /* Contadores: "  Ocup: X/Y  Livres: Z  (PP%)" */
    {
        char buf[64];
        snprintf(buf, sizeof(buf), "  Ocup: %d/%d  Livres: %d  (%d%%)",
                 ocupadas, total, livres, pct);
        int vis = (int)strlen(buf);
        int pad = W - vis;
        if (pad < 0) pad = 0;
        printf(COR_BARRA "  ║" COR_RESET COR_NEGRITO "%s" COR_RESET, buf);
        for (int i = 0; i < pad; i++) printf(" ");
        printf(COR_BARRA "║\n" COR_RESET);
    }

    printf(COR_BARRA "  ╚"); borda_h(); printf("╝\n\n" COR_RESET);
}

/* ------------------------------------------------------------------ */
/*  exibir_relatorio_final                                              */
/* ------------------------------------------------------------------ */

void exibir_relatorio_final()
{
    long media = stats.total_saidas > 0
                 ? stats.tempo_total_estacionado / stats.total_saidas
                 : 0;

    printf(COR_TITULO "\n  ╔"); borda_h(); printf("╗\n" COR_RESET);

    /* Título */
    {
        const char *t = "RELATORIO FINAL DA SIMULACAO";
        int vis = (int)strlen(t);
        int pad = W - vis;
        int lp  = pad / 2, rp = pad - lp;
        printf(COR_TITULO "  ║" COR_NEGRITO);
        for(int i=0;i<lp;i++) printf(" ");
        printf("%s", t);
        for(int i=0;i<rp;i++) printf(" ");
        printf(COR_TITULO "║\n" COR_RESET);
    }

    /* --- Fluxo --- */
    {
        int pad = W - (int)strlen(" Fluxo de veiculos");
        printf(COR_TITULO "  ╠"); borda_h(); printf("╣\n" COR_RESET);
        printf(COR_TITULO "  ║" COR_RELATORIO COR_NEGRITO " Fluxo de veiculos" COR_RESET);
        for(int i=0;i<pad;i++) printf(" ");
        printf(COR_TITULO "║\n" COR_RESET);
    }
    linha_int("Total de entradas",  stats.total_entradas);
    linha_int("Total de saidas",    stats.total_saidas);
    linha_int("Pico de ocupacao",   stats.pico_vagas);

    /* --- PCD --- */
    {
        int pad = W - (int)strlen(" Veiculos PCD");
        printf(COR_TITULO "  ╠"); borda_h(); printf("╣\n" COR_RESET);
        printf(COR_TITULO "  ║" COR_RELATORIO COR_NEGRITO " Veiculos PCD" COR_RESET);
        for(int i=0;i<pad;i++) printf(" ");
        printf(COR_TITULO "║\n" COR_RESET);
    }
    linha_int("Total PCD",           stats.total_pcd);
    linha_int("Usaram vaga PCD",     stats.total_pcd_vaga_reservada);
    linha_int("Usaram vaga comum",   stats.total_pcd_vaga_comum);

    /* --- Tempo --- */
    {
        int pad = W - (int)strlen(" Tempo estacionado");
        printf(COR_TITULO "  ╠"); borda_h(); printf("╣\n" COR_RESET);
        printf(COR_TITULO "  ║" COR_RELATORIO COR_NEGRITO " Tempo estacionado" COR_RESET);
        for(int i=0;i<pad;i++) printf(" ");
        printf(COR_TITULO "║\n" COR_RESET);
    }
    linha_long("Tempo medio",  media);
    linha_long("Tempo minimo", stats.tempo_min);
    linha_long("Tempo maximo", stats.tempo_max);

    /* --- Uso por vaga --- */
    {
        int pad = W - (int)strlen(" Uso por vaga");
        printf(COR_TITULO "  ╠"); borda_h(); printf("╣\n" COR_RESET);
        printf(COR_TITULO "  ║" COR_RELATORIO COR_NEGRITO " Uso por vaga" COR_RESET);
        for(int i=0;i<pad;i++) printf(" ");
        printf(COR_TITULO "║\n" COR_RESET);
    }

    for (int i = 0; i < TOTAL_VAGAS; i++)
    {
        int usos   = stats.usos_por_vaga[i];
        int blocos = usos < 16 ? usos : 16;
        const char *cor_v = vagas[i].pcd ? COR_VAGA_PCD : COR_VAGA;

        /* texto visível: "  C1 : ### N usos" */
        int digits = usos >= 10 ? 2 : 1;
        int vis = 2 + 2 + 3 + blocos + 1 + digits + 5;
        int pad = W - vis;
        if (pad < 0) pad = 0;

        printf(COR_TITULO "  ║" COR_RESET "  %s%-2s" COR_RESET " : ",
               cor_v, vagas[i].nome);
        for (int b = 0; b < blocos; b++)
            printf("%s█" COR_RESET, cor_v);
        printf(" %d usos", usos);
        for (int p = 0; p < pad; p++) printf(" ");
        printf(COR_TITULO "║\n" COR_RESET);
    }

    printf(COR_TITULO "  ╚"); borda_h(); printf("╝\n\n" COR_RESET);
}
