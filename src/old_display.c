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

/* Linha em branco dentro da caixa */

/*
 * Imprime uma linha com texto já formatado (sem cores) centralizado/esquerda.
 * text_vis = quantidade de caracteres VISÍVEIS no conteúdo (sem códigos ANSI).
 * conteudo  = string com cores para imprimir.
 * O padding à direita é calculado com base em text_vis.
 */


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

    /* Título centralizado: "  MAPA DE VAGAS  " — 14 chars visíveis com margens */
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

    /* Vagas comuns: [C1] [C2] ... [C8]
       cada vaga: "[C1] " = 5 chars, 8 vagas = 40 chars + 2 margem = 42 vis */
    printf(COR_BARRA "  ║" COR_RESET " ");
    for (int i = 0; i < VAGAS_COMUNS; i++)
    {
        if (vagas[i].ocupada)
            printf(COR_SAIDA "[%s]" COR_RESET " ", vagas[i].nome);
        else
            printf(COR_VAGA  "[%s]" COR_RESET " ", vagas[i].nome);
    }
    /* vis = 1 + 8*(4+1) = 1+40 = 41, pad = 46-41 = 5 */
    printf("     " COR_BARRA "║\n" COR_RESET);

    /* Vagas PCD: "PCD: [P1] [P2] "
       vis = "PCD: " (5) + 2*(4+1) (10) + 1 margem = 16, pad = 30 */
    printf(COR_BARRA "  ║" COR_RESET " " COR_PCD "PCD:" COR_RESET " ");
    for (int i = VAGAS_COMUNS; i < TOTAL_VAGAS; i++)
    {
        if (vagas[i].ocupada)
            printf(COR_SAIDA    "[%s]" COR_RESET " ", vagas[i].nome);
        else
            printf(COR_VAGA_PCD "[%s]" COR_RESET " ", vagas[i].nome);
    }
    /* vis = 1+"PCD: "(5)+2*(4+1)(10)+1 = 17, pad = 46-17 = 29 */
    printf("                             " COR_BARRA "║\n" COR_RESET);

    printf(COR_BARRA "  ╠"); borda_h(); printf("╣\n" COR_RESET);

    /* Barra de progresso:
     * █ ocupa 2 colunas no terminal → usamos W/2 blocos (23).
     * Cada bloco cheio imprime "█" (2 cols) e cada vazio imprime
     * "  " (2 espaços) para manter largura total = W colunas. */
    int bar_n     = W / 2;                          /* 23 blocos */
    int bar_cheio = (ocupadas * bar_n) / total;
    const char *cor_bloco = pct >= 90 ? COR_SAIDA
                          : pct >= 60 ? "\033[1;33m"
                                      : COR_ENTRADA;

    printf(COR_BARRA "  ║" COR_RESET);
    for (int i = 0; i < bar_n; i++)
    {
        if (i < bar_cheio)
            printf("%s█" COR_RESET, cor_bloco);
        else
            printf(COR_ESPERA "░" COR_RESET);
    }
    printf(COR_BARRA "║\n" COR_RESET);

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

        /* texto visível: "  C1 : ### N usos"
           "  " (2) + nome (2) + " : " (3) + blocos + " " (1) + "N usos" (6+digitos) */
        int digits = usos >= 10 ? 2 : 1;
        int vis = 2 + 2 + 3 + blocos + 1 + digits + 5; /* "X usos" */
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
