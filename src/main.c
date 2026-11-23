/**
 * @file main.c
 * @brief Ponto de entrada do WMLETRAS — controla o menu principal e o fluxo do jogo.
 *
 * Autor:      Washington Matheus Almeida Xavier
 * Matricula:  252024622
 * Disciplina: Algoritmos e Programacao de Computadores (APC)
 *
 * Referências de aprendizado usadas durante o desenvolvimento:
 *   - Abertura de arquivos em C:   https://youtu.be/MtYuhRtQPHU
 *   - Limpar tela via ANSI:        https://stackoverflow.com/q/55672661
 *   - Cores no terminal:           https://youtu.be/Hl-7xMl7bn0
 */
#include "ui.h"
#include "jogo.h"
#include "ranking.h"
#include "tipos.h"

#include <ncurses.h>
#include <stdio.h>
#include <string.h>

/* Caminhos dos arquivos de dados (relativos ao diretório de execução) */
#define ARQ_FASES   "data/entrada.txt"
#define ARQ_RANKING "ranking.bin"

/* ── Easter egg ─────────────────────────────────────────────────────────── */

#define EASTER_EGG_CODIGO "67"
#define EASTER_EGG_BONUS   9999

static void tela_easter_egg(EstadoJogo *estado) {
    int marg = (estado->largura_tela - 37) / 2;
    if (marg < 0) marg = 0;

    clear();
    attron(COLOR_PAIR(PAR_VERDE) | A_BOLD);
    printw("%*s%s\n", marg, "", "0101010101010101010101010101010101010");
    printw("%*s%s\n", marg, "", "10101010101010 PARABENS! 010101010101");
    printw("%*s%s\n", marg, "", "0101010101010101010101010101010101010");
    printw("%*s%s\n", marg, "", "10101010 CODIGO SECRETO ATIVADO 01010");
    printw("%*s%s\n", marg, "", "0101010101010101010101010101010101010");

    char bonus_msg[50];
    snprintf(bonus_msg, sizeof(bonus_msg),
             "10101010  +%d PONTOS NO PROXIMO JOGO  ", EASTER_EGG_BONUS);
    printw("%*s%s\n", marg, "", bonus_msg);

    printw("%*s%s\n", marg, "", "1010101010101010101010101010101010101");
    attroff(COLOR_PAIR(PAR_VERDE) | A_BOLD);
    refresh();

    estado->bonus_easter_egg += EASTER_EGG_BONUS;
    ui_pausar(estado->largura_tela);
}

/* ── Tela de configuração do modo desafio ──────────────────────────────── */

static void tela_modo_desafio(EstadoJogo *estado) {
    int largura = estado->largura_tela;
    int marg    = (largura - 37) / 2;
    if (marg < 0) marg = 0;

    while (1) {
        clear();
        attron(COLOR_PAIR(PAR_AMARELO) | A_BOLD);
        printw("%*s%s\n", marg, "", "+-----------------------------------+");
        printw("%*s%s\n", marg, "", "|        MODO DESAFIO               |");
        printw("%*s%s\n", marg, "", "+-----------------------------------+");
        printw("%*s%s\n", marg, "", "|                                   |");

        /* Status atual */
        {
            char linha[42];
            snprintf(linha, sizeof(linha), "| Cronometro: %-22s|",
                     estado->modo_desafio ? "ATIVO" : "inativo");
            printw("%*s%s\n", marg, "", linha);
        }
        {
            char linha[42];
            snprintf(linha, sizeof(linha), "| Tempo/fase: %-3d s               |",
                     estado->modo_desafio
                     ? (estado->tempo_por_fase > 0 ? estado->tempo_por_fase : TEMPO_PADRAO_FASE)
                     : TEMPO_PADRAO_FASE);
            printw("%*s%s\n", marg, "", linha);
        }
        {
            char linha[42];
            snprintf(linha, sizeof(linha), "| Dicionario: %-22s|",
                     estado->usar_dicionario ? "ATIVO" : "inativo");
            printw("%*s%s\n", marg, "", linha);
        }
        printw("%*s%s\n", marg, "", "|                                   |");
        printw("%*s%s\n", marg, "", "| 1 - Ligar/desligar cronometro     |");
        printw("%*s%s\n", marg, "", "| 2 - Alterar tempo (30/45/60/90 s) |");
        printw("%*s%s\n", marg, "", "| 3 - Ligar/desligar dicionario     |");
        printw("%*s%s\n", marg, "", "| 0 - Voltar                        |");
        printw("%*s%s\n", marg, "", "+-----------------------------------+");
        attroff(COLOR_PAIR(PAR_AMARELO) | A_BOLD);

        attron(COLOR_PAIR(PAR_NORMAL));
        ui_centralizado(largura, "Escolha: ");
        attroff(COLOR_PAIR(PAR_NORMAL));
        refresh();

        int ch = getch();

        if (ch == '1') {
            estado->modo_desafio = !estado->modo_desafio;
        } else if (ch == '2') {
            /* Alterna ciclo 30 -> 45 -> 60 -> 90 -> 30 */
            int t = (estado->tempo_por_fase > 0)
                    ? estado->tempo_por_fase : TEMPO_PADRAO_FASE;
            if      (t <  30) t = 30;
            else if (t <  45) t = 45;
            else if (t <  60) t = 60;
            else if (t <  90) t = 90;
            else              t = 30;
            estado->tempo_por_fase = t;
        } else if (ch == '3') {
            estado->usar_dicionario = !estado->usar_dicionario;
        } else if (ch == '0' || ch == 'q' || ch == 'Q') {
            return;
        }
    }
}

/* ── Menu de configurações ──────────────────────────────────────────────── */

static void tela_configuracoes(EstadoJogo *estado) {
    int largura = estado->largura_tela;
    int marg    = (largura - 37) / 2;
    if (marg < 0) marg = 0;

    char buf[16];

    while (1) {
        clear();
        attron(COLOR_PAIR(PAR_VERDE) | A_BOLD);
        printw("%*s%s\n", marg, "", "+-----------------------------------+");
        printw("%*s%s\n", marg, "", "|        CONFIGURACOES              |");
        printw("%*s%s\n", marg, "", "+-----------------------------------+");
        printw("%*s%s\n", marg, "", "|  1. Zerar ranking                 |");
        printw("%*s%s\n", marg, "", "|  2. Voltar ao menu                |");
        printw("%*s%s\n", marg, "", "+-----------------------------------+");
        attroff(A_BOLD);
        attron(COLOR_PAIR(PAR_NORMAL));
        ui_centralizado(largura, "Escolha (ou '67' para codigo secreto): ");
        attroff(COLOR_PAIR(PAR_NORMAL));
        refresh();

        /* Lê até 2 chars para detectar easter egg "67" */
        buf[0] = '\0';
        ui_ler_linha(buf, 2);

        /* Easter egg */
        if (buf[0] == EASTER_EGG_CODIGO[0] && buf[1] == EASTER_EGG_CODIGO[1]) {
            tela_easter_egg(estado);
            continue;
        }

        if (buf[0] == '1') {
            static const ItemMenu conf[] = {
                { "SIM, apagar tudo" },
                { "NAO, cancelar"    },
            };
            clear();
            printw("\n\n");
            attron(COLOR_PAIR(PAR_VERMELHO) | A_BOLD);
            ui_centralizado(largura, "ESSA ACAO NAO PODE SER DESFEITA!\n\n");
            attroff(COLOR_PAIR(PAR_VERMELHO) | A_BOLD);
            refresh();
            int c = ui_menu("TEM CERTEZA?", conf, 2, 1, largura);
            if (c == 0) {
                ranking_apagar(ARQ_RANKING);
                clear();
                printw("\n\n\n");
                attron(COLOR_PAIR(PAR_VERDE) | A_BOLD);
                ui_centralizado(largura, "Ranking zerado com sucesso.\n");
                attroff(COLOR_PAIR(PAR_VERDE) | A_BOLD);
                refresh();
                ui_pausar(largura);
            }
            return;
        }

        if (buf[0] == '2' || buf[0] == '\0') return;
    }
}

/* ── Menu "confirmar saída" ─────────────────────────────────────────────── */

static int confirmar_saida(int largura) {
    static const ItemMenu itens[] = {
        { "Sim, sair"    },
        { "Nao, voltar"  },
    };
    int r = ui_menu("TEM CERTEZA QUE DESEJA SAIR?", itens, 2, 1, largura);
    return (r == 0) ? 1 : 0;
}

/* ── Loop principal ─────────────────────────────────────────────────────── */

static void loop_principal(EstadoJogo *estado) {
    static const ItemMenu itens_menu[] = {
        { "Jogar (modo normal)"    },
        { "Jogar (modo desafio)"   },
        { "Configurar desafio"     },
        { "Instrucoes"             },
        { "Ranking"                },
        { "Configuracoes"          },
        { "Sair"                   },
    };

    int sair = 0;
    while (!sair) {
        int op = ui_menu("WMLETRAS", itens_menu, 7, 6, estado->largura_tela);
        switch (op) {
            case 0:
                /* Modo normal: garante que modo_desafio=0 para esta partida */
                estado->modo_desafio = 0;
                jogo_jogar(estado, ARQ_FASES, ARQ_RANKING);
                break;
            case 1:
                /* Modo desafio: ativa o cronômetro */
                estado->modo_desafio = 1;
                if (estado->tempo_por_fase <= 0)
                    estado->tempo_por_fase = TEMPO_PADRAO_FASE;
                jogo_jogar(estado, ARQ_FASES, ARQ_RANKING);
                break;
            case 2:
                tela_modo_desafio(estado);
                break;
            case 3:
                ui_instrucoes(estado->largura_tela);
                break;
            case 4:
                ranking_exibir(ARQ_RANKING, estado->largura_tela);
                ui_pausar(estado->largura_tela);
                break;
            case 5:
                tela_configuracoes(estado);
                break;
            case 6:
                sair = confirmar_saida(estado->largura_tela);
                break;
            default:
                break;
        }
    }
}

/* ── main ───────────────────────────────────────────────────────────────── */

int main(void) {
    /* Inicializa ncurses ANTES de qualquer saída */
    ui_iniciar();

    EstadoJogo estado;
    memset(&estado, 0, sizeof(estado));
    estado.largura_tela    = ui_largura_terminal();
    estado.tempo_por_fase  = TEMPO_PADRAO_FASE;
    estado.usar_dicionario = 1;   /* dicionário ativo por padrão */

    ui_tela_boas_vindas(&estado);
    loop_principal(&estado);
    ui_tela_fim(estado.largura_tela);

    /* Restaura o terminal */
    ui_encerrar();

    return 0;
}
