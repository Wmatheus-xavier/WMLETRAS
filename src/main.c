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

#include <stdio.h>
#include <string.h>

/* Caminhos dos arquivos de dados (relativos ao diretório de execução) */
#define ARQ_FASES   "data/entrada.txt"
#define ARQ_RANKING "ranking.bin"

/* ── Easter egg ─────────────────────────────────────────────────────────── */

/*
 * O easter egg é ativado na tela de Configurações digitando "67" + ENTER.
 * Ele adiciona pontos ao estado ANTES de iniciar a partida, então os pontos
 * persistem no jogo e são salvos no ranking.
 *
 * Por que "67"? Washington = 23ª letra, Matheus = 13ª → 23+13 = 36… ou
 * talvez seja só o ano favorito do autor. Descubra você mesmo. ;)
 */
#define EASTER_EGG_CODIGO "67"
#define EASTER_EGG_BONUS   9999

static void tela_easter_egg(EstadoJogo *estado) {
    int marg = (estado->largura_tela - 37) / 2;
    if (marg < 0) marg = 0;

    ui_limpar_tela();
    printf("%s", COR_VERDE);
    printf("%*s%s\n", marg, "", "0101010101010101010101010101010101010");
    printf("%*s%s\n", marg, "", "10101010101010 PARABENS! 010101010101");
    printf("%*s%s\n", marg, "", "0101010101010101010101010101010101010");
    printf("%*s%s\n", marg, "", "10101010 CODIGO SECRETO ATIVADO 01010");
    printf("%*s%s\n", marg, "", "0101010101010101010101010101010101010");

    char bonus_msg[50];
    snprintf(bonus_msg, sizeof(bonus_msg),
             "10101010  +%d PONTOS NO PROXIMO JOGO  ", EASTER_EGG_BONUS);
    printf("%*s%s\n", marg, "", bonus_msg);

    printf("%*s%s\n", marg, "", "1010101010101010101010101010101010101");
    printf("%s", COR_RESET);

    estado->pontos += EASTER_EGG_BONUS;
    ui_pausar(estado->largura_tela);
}

/* ── Menu de configurações ──────────────────────────────────────────────── */

static void tela_configuracoes(EstadoJogo *estado) {
    static const ItemMenu itens[] = {
        { "Zerar ranking" },
        { "Voltar ao menu" },
    };

    char linha_raw[16];
    int  largura = estado->largura_tela;
    int  marg    = (largura - 37) / 2;
    if (marg < 0) marg = 0;

    while (1) {
        /* Reusa ui_menu mas precisamos verificar o easter egg antes */
        ui_limpar_tela();
        printf("%s", COR_VERDE);
        printf("%*s%s\n", marg, "", "+-----------------------------------+");
        printf("%*s%s\n", marg, "", "|        CONFIGURACOES              |");
        printf("%*s%s\n", marg, "", "+-----------------------------------+");
        printf("%*s%s\n", marg, "", "|  1. Zerar ranking                 |");
        printf("%*s%s\n", marg, "", "|  2. Voltar ao menu                |");
        printf("%*s%s\n", marg, "", "+-----------------------------------+");
        ui_centralizado(largura, "w=sobe  s=desce  f=confirma\n");
        ui_centralizado(largura, "> ");

        if (fgets(linha_raw, sizeof(linha_raw), stdin) == NULL) return;

        /* Easter egg: digitar "67" + ENTER */
        if (linha_raw[0] == EASTER_EGG_CODIGO[0] &&
            linha_raw[1] == EASTER_EGG_CODIGO[1]) {
            tela_easter_egg(estado);
            continue;
        }

        int opcao = ui_menu("CONFIGURACOES", itens, 2, 1, largura);
        if (opcao == 0) {
            /* Confirmação antes de apagar */
            static const ItemMenu conf[] = {
                { "SIM, apagar tudo" },
                { "NAO, cancelar"    },
            };
            ui_limpar_tela();
            printf("\n\n%s", COR_VERMELHO);
            ui_centralizado(largura, "ESSA ACAO NAO PODE SER DESFEITA!\n\n");
            printf("%s", COR_RESET);
            int c = ui_menu("TEM CERTEZA?", conf, 2, 1, largura);
            if (c == 0) {
                ranking_apagar(ARQ_RANKING);
                ui_limpar_tela();
                printf("\n\n\n%s", COR_VERDE);
                ui_centralizado(largura, "Ranking zerado com sucesso.\n");
                printf("%s", COR_RESET);
                ui_pausar(largura);
            }
        }
        return; /* qualquer opção válida sai da tela de config */
    }
}

/* ── Menu "confirmar saída" ─────────────────────────────────────────────── */

static int confirmar_saida(int largura) {
    static const ItemMenu itens[] = {
        { "Sim, sair" },
        { "Nao, voltar" },
    };
    int r = ui_menu("TEM CERTEZA QUE DESEJA SAIR?", itens, 2, 1, largura);
    return (r == 0) ? 1 : 0;
}

/* ── Loop principal ─────────────────────────────────────────────────────── */

static void loop_principal(EstadoJogo *estado) {
    static const ItemMenu itens_menu[] = {
        { "Jogar"          },
        { "Instrucoes"     },
        { "Ranking"        },
        { "Configuracoes"  },
        { "Sair"           },
    };

    int sair = 0;
    while (!sair) {
        int op = ui_menu("WMLETRAS", itens_menu, 5, 4, estado->largura_tela);
        switch (op) {
            case 0: jogo_jogar(estado, ARQ_FASES, ARQ_RANKING);       break;
            case 1: ui_instrucoes(estado->largura_tela);               break;
            case 2: ranking_exibir(ARQ_RANKING, estado->largura_tela);
                    ui_pausar(estado->largura_tela);                   break;
            case 3: tela_configuracoes(estado);                        break;
            case 4: sair = confirmar_saida(estado->largura_tela);      break;
            default: break;
        }
    }
}

/* ── main ───────────────────────────────────────────────────────────────── */

int main(void) {
    EstadoJogo estado;
    memset(&estado, 0, sizeof(estado));
    estado.largura_tela = ui_largura_terminal();

    ui_tela_boas_vindas(&estado);
    loop_principal(&estado);
    ui_tela_fim(estado.largura_tela);

    return 0;
}
