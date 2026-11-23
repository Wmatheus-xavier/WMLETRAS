/**
 * @file jogo.c
 * @brief Lógica principal da partida do WMLETRAS.
 */
#include "jogo.h"
#include "ui.h"
#include "fases.h"
#include "ranking.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* ── Exibe o estado atual da fase ───────────────────────────────────────── */

static void exibir_fase(const EstadoJogo *estado, int fase_idx,
                        int palavras_restantes) {
    int    largura  = estado->largura_tela;
    Fase  *fase     = (Fase *)&estado->fases[fase_idx];

    ui_limpar_tela();
    printf("%s", COR_VERDE);

    /* HUD: informações da partida */
    printf("\n");
    /* Nome truncado a 20 chars para caber no HUD */
    char hud[160];
    snprintf(hud, sizeof(hud),
             "  Fase %d/%d  |  Palavras: %d/%d  |  Pontos: %d  |  Jogador: %.20s",
             fase_idx + 1, estado->qtd_fases,
             fase->total - palavras_restantes, fase->total,
             estado->pontos,
             estado->nome_jogador);
    printf("%s\n", hud);
    printf("\n");

    /* Título da fase */
    char titulo[60];
    snprintf(titulo, sizeof(titulo), "--- FASE %d ---", fase_idx + 1);
    ui_centralizado(largura, titulo);
    printf("\n\n");

    /* Letras disponíveis */
    ui_centralizado(largura, "LETRAS DISPONIVEIS:\n\n");

    char visual[200] = "";
    for (int k = 0; fase->letras[k] != '\0'; k++) {
        char tmp[6];
        if (fase->letras[k + 1] != '\0')
            snprintf(tmp, sizeof(tmp), "%c -- ", fase->letras[k]);
        else
            snprintf(tmp, sizeof(tmp), "%c", fase->letras[k]);
        strncat(visual, tmp, sizeof(visual) - strlen(visual) - 1);
    }
    ui_centralizado(largura, visual);
    printf("\n\n");

    /* Lista de palavras: reveladas se acertadas, ocultas caso contrário */
    int marg_slot = (largura - 14) / 2;
    if (marg_slot < 0) marg_slot = 0;
    for (int j = 0; j < fase->total; j++) {
        if (fase->acertos[j])
            printf("%*s[ %-10s ]\n", marg_slot, "", fase->respostas[j]);
        else
            printf("%*s[ %-10s ]\n", marg_slot, "", "??????");
    }
    printf("\n");
}

/* ── Loop principal da partida ──────────────────────────────────────────── */

void jogo_jogar(EstadoJogo *estado,
                const char *arq_fases,
                const char *arq_rank) {
    int largura = estado->largura_tela;

    /* Carrega fases */
    if (fases_carregar(arq_fases, estado) == 0) {
        ui_limpar_tela();
        printf("%s\n\n", COR_VERMELHO);
        printf("  ERRO: arquivo '%s' nao encontrado ou vazio.\n", arq_fases);
        printf("  Certifique-se de que o arquivo esta na pasta correta.\n\n");
        printf("%s", COR_RESET);
        ui_pausar(largura);
        return;
    }

    /* Inicializa a partida — aplica bônus do easter egg (se houver) e zera a flag */
    fases_reiniciar_acertos(estado);
    estado->pontos            = estado->bonus_easter_egg;
    estado->bonus_easter_egg  = 0;

    ui_tela_inicio_jogo(largura);

    char tentativa[MAX_PALAVRA];
    int  fase_idx = 0;

    while (fase_idx < estado->qtd_fases) {
        Fase *fase = &estado->fases[fase_idx];
        int   restantes = fase->total;

        while (restantes > 0) {
            exibir_fase(estado, fase_idx, restantes);

            ui_centralizado(largura,
                "Digite a palavra (ou SAIR para salvar e sair): ");

            if (!ui_ler_linha(tentativa, MAX_PALAVRA - 1)) {
                /* EOF — salva e encerra */
                ranking_salvar(arq_rank, estado->nome_jogador, estado->pontos);
                return;
            }

            if (tentativa[0] == '\0') continue; /* linha vazia */

            /* Converte para maiúsculas */
            for (int k = 0; tentativa[k]; k++)
                tentativa[k] = (char)toupper((unsigned char)tentativa[k]);

            if (strcmp(tentativa, "SAIR") == 0) {
                ranking_salvar(arq_rank, estado->nome_jogador, estado->pontos);
                return;
            }

            /* Verifica contra as respostas da fase */
            int acertou = 0;
            for (int j = 0; j < fase->total; j++) {
                if (fase->acertos[j] == 0 &&
                    strcmp(tentativa, fase->respostas[j]) == 0) {
                    fase->acertos[j] = 1;
                    restantes--;
                    acertou = 1;
                    break;
                }
            }

            if (acertou) {
                estado->pontos += PTS_ACERTO;
                printf("\n%s", COR_VERDE);
                ui_centralizado(largura, "BOA! +100 pontos!\n");
                printf("%s", COR_RESET);
                ui_pausar(largura);
            } else {
                estado->pontos -= PTS_ERRO;
                if (estado->pontos < 0) estado->pontos = 0;
                printf("\n%s", COR_VERMELHO);
                ui_centralizado(largura,
                    "Palavra incorreta ou ja encontrada! -10 pontos.\n");
                printf("%s", COR_RESET);
                ui_pausar(largura);
            }
        }

        /* Fase concluída */
        estado->pontos += PTS_FASE;
        ui_limpar_tela();
        printf("\n\n\n%s", COR_VERDE);
        char msg_fase[80];
        snprintf(msg_fase, sizeof(msg_fase),
                 "FASE %d CONCLUIDA! +25 pontos. Total: %d pts\n",
                 fase_idx + 1, estado->pontos);
        ui_centralizado(largura, msg_fase);
        printf("%s", COR_RESET);
        ui_pausar(largura);
        fase_idx++;
    }

    /* Zerou o jogo */
    ui_limpar_tela();
    printf("\n\n\n%s", COR_CIANO);
    ui_centralizado(largura, "PARABENS! VOCE ZEROU O WMLETRAS!\n\n");
    char msg_final[60];
    snprintf(msg_final, sizeof(msg_final),
             "Pontuacao final: %d pts\n", estado->pontos);
    ui_centralizado(largura, msg_final);
    printf("%s", COR_RESET);

    ranking_salvar(arq_rank, estado->nome_jogador, estado->pontos);
    ui_pausar(largura);
}
