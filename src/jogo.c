/**
 * @file jogo.c
 * @brief Lógica principal da partida do WMLETRAS.
 *
 * Novidades:
 *  - Modo desafio: cronômetro regressivo por fase; acertos rápidos dão bônus.
 *  - Dicionário: tentativas que não sejam palavras reais são rejeitadas sem
 *    penalidade de pontos.
 *  - Interface ncurses: HUD atualizado ao vivo a cada segundo do cronômetro.
 */
#include "jogo.h"
#include "ui.h"
#include "fases.h"
#include "ranking.h"
#include "dict.h"

#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

/* ── Contexto passado ao callback de tick do cronômetro ─────────────────── */

typedef struct {
    const EstadoJogo *estado;
    int               fase_idx;
    int               palavras_restantes;
    int               tempo_total;       /* segundos configurados para a fase */
    /* Posição na tela onde o HUD do cronômetro é desenhado */
    int               row_hud;
    int               col_timer;
} CtxTick;

/* ── Helpers de cor ncurses ─────────────────────────────────────────────── */

static int par_timer(int restantes, int total) {
    if (restantes > total / 2) return PAR_VERDE;
    if (restantes > total / 4) return PAR_AMARELO;
    return PAR_VERMELHO;
}

/* ── Cálculo de bônus por velocidade ────────────────────────────────────── */

/*
 * Quanto mais tempo restante, maior o bônus.
 * Escala linear de 0 a BONUS_RAPIDO_MAX conforme restantes / tempo_total.
 * Com 0 s restantes: 0 pts.  Com 100% restantes: BONUS_RAPIDO_MAX pts.
 */
static int calcular_bonus(int restantes, int tempo_total) {
    if (tempo_total <= 0 || restantes <= 0) return 0;
    int bonus = (int)((long)restantes * BONUS_RAPIDO_MAX / tempo_total);
    if (bonus > BONUS_RAPIDO_MAX) bonus = BONUS_RAPIDO_MAX;
    return bonus;
}

/* ── Exibe o HUD completo da fase ───────────────────────────────────────── */

static void exibir_fase(const EstadoJogo *estado, int fase_idx,
                        int palavras_restantes, int segundos_restantes) {
    int    largura = estado->largura_tela;
    Fase  *fase    = (Fase *)&estado->fases[fase_idx];

    clear();
    attron(COLOR_PAIR(PAR_VERDE) | A_BOLD);

    /* ── Linha 0: HUD ─────────────────────────────────────────────────── */
    char hud[160];
    if (estado->modo_desafio && segundos_restantes >= 0) {
        int par = par_timer(segundos_restantes, estado->tempo_por_fase);
        snprintf(hud, sizeof(hud),
                 "  Fase %d/%d  |  Palavras: %d/%d  |  Pontos: %-5d  |  Jogador: %.18s",
                 fase_idx + 1, estado->qtd_fases,
                 fase->total - palavras_restantes, fase->total,
                 estado->pontos,
                 estado->nome_jogador);
        attron(COLOR_PAIR(PAR_VERDE) | A_BOLD);
        printw("\n%s", hud);

        /* Cronômetro em cor separada */
        printw("  ");
        attron(COLOR_PAIR(par) | A_BOLD);
        printw("[TEMPO: %3d s]", segundos_restantes);
        attroff(COLOR_PAIR(par) | A_BOLD);
        printw("\n");
    } else {
        snprintf(hud, sizeof(hud),
                 "  Fase %d/%d  |  Palavras: %d/%d  |  Pontos: %-5d  |  Jogador: %.20s",
                 fase_idx + 1, estado->qtd_fases,
                 fase->total - palavras_restantes, fase->total,
                 estado->pontos,
                 estado->nome_jogador);
        printw("\n%s\n", hud);
    }

    /* ── Título ──────────────────────────────────────────────────────── */
    printw("\n");
    char titulo[60];
    snprintf(titulo, sizeof(titulo), "--- FASE %d ---", fase_idx + 1);
    attron(COLOR_PAIR(PAR_VERDE) | A_BOLD);
    ui_centralizado(largura, titulo);
    printw("\n\n");

    /* ── Letras disponíveis ──────────────────────────────────────────── */
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
    printw("\n\n");

    /* ── Lista de palavras ───────────────────────────────────────────── */
    int marg_slot = (largura - 14) / 2;
    if (marg_slot < 0) marg_slot = 0;
    for (int j = 0; j < fase->total; j++) {
        if (fase->acertos[j]) {
            attron(COLOR_PAIR(PAR_CIANO) | A_BOLD);
            printw("%*s[ %-10s ]\n", marg_slot, "", fase->respostas[j]);
        } else {
            attron(COLOR_PAIR(PAR_VERDE));
            printw("%*s[ %-10s ]\n", marg_slot, "", "??????");
        }
    }
    attron(COLOR_PAIR(PAR_VERDE) | A_BOLD);
    printw("\n");

    /* ── Info de modo ───────────────────────────────────────────────── */
    if (estado->usar_dicionario) {
        attron(COLOR_PAIR(PAR_AMARELO));
        ui_centralizado(largura, "[Dicionario ativo: apenas palavras reais sao aceitas]\n");
        attroff(COLOR_PAIR(PAR_AMARELO));
    }

    refresh();
}

/* ── Callback de tick do cronômetro ─────────────────────────────────────── */

/*
 * Chamado a cada segundo por ui_ler_linha_timeout().
 * Atualiza apenas a porção do HUD que mostra o tempo restante,
 * evitando redesenhar toda a tela.
 */
static void tick_hud(int segundos_restantes, void *dados) {
    CtxTick *ctx = (CtxTick *)dados;
    int par = par_timer(segundos_restantes, ctx->tempo_total);

    /* Salva posição atual (onde o usuário está digitando) */
    int row_atual, col_atual;
    getyx(stdscr, row_atual, col_atual);

    /* Atualiza o campo [TEMPO: xxx s] na linha do HUD */
    attron(COLOR_PAIR(par) | A_BOLD);
    mvprintw(ctx->row_hud, ctx->col_timer, "[TEMPO: %3d s]", segundos_restantes);
    attroff(A_BOLD);

    /* Restaura posição do cursor */
    move(row_atual, col_atual);
    refresh();
}

/* ── Loop principal da partida ──────────────────────────────────────────── */

void jogo_jogar(EstadoJogo *estado,
                const char *arq_fases,
                const char *arq_rank) {
    int largura = estado->largura_tela;

    /* Carrega fases */
    if (fases_carregar(arq_fases, estado) == 0) {
        clear();
        attron(COLOR_PAIR(PAR_VERMELHO) | A_BOLD);
        printw("\n\n  ERRO: arquivo '%s' nao encontrado ou vazio.\n", arq_fases);
        printw("  Certifique-se de que o arquivo esta na pasta correta.\n\n");
        attroff(COLOR_PAIR(PAR_VERMELHO) | A_BOLD);
        refresh();
        ui_pausar(largura);
        return;
    }

    /* Inicializa a partida */
    fases_reiniciar_acertos(estado);
    estado->pontos           = estado->bonus_easter_egg;
    estado->bonus_easter_egg = 0;

    /* Tempo por fase: usa configurado ou padrão */
    int tempo_fase = (estado->tempo_por_fase > 0)
                      ? estado->tempo_por_fase
                      : TEMPO_PADRAO_FASE;

    ui_tela_inicio_jogo(largura);

    char tentativa[MAX_PALAVRA];
    int  fase_idx = 0;

    while (fase_idx < estado->qtd_fases) {
        Fase *fase     = &estado->fases[fase_idx];
        int   restantes = fase->total;

        /* Tempo inicial para a fase (modo desafio) */
        time_t fase_inicio = time(NULL);

        while (restantes > 0) {
            /* Calcula segundos restantes antes de exibir */
            int seg_restantes = -1; /* -1 = modo normal sem timer */
            if (estado->modo_desafio) {
                int decorrido = (int)(time(NULL) - fase_inicio);
                seg_restantes = tempo_fase - decorrido;
                if (seg_restantes < 0) seg_restantes = 0;
            }

            exibir_fase(estado, fase_idx, restantes, seg_restantes);

            /* Verifica timeout antes de pedir input */
            if (estado->modo_desafio && seg_restantes <= 0) {
                /* Tempo esgotado — avança a fase sem bônus */
                attron(COLOR_PAIR(PAR_VERMELHO) | A_BOLD);
                printw("\n");
                ui_centralizado(largura, "TEMPO ESGOTADO! Avancando para proxima fase...\n");
                attroff(COLOR_PAIR(PAR_VERMELHO) | A_BOLD);
                refresh();
                ui_pausar(largura);
                goto proxima_fase;
            }

            ui_centralizado(largura,
                "Digite a palavra (ou SAIR para salvar e sair): ");
            refresh();

            /* Calcula coluna onde começa o campo [TEMPO: xxx s] no HUD
             * Linha 1 (índice 0 = printw, 1 = HUD) — a linha \n do HUD é
             * impressa na linha 1 da tela ncurses.
             * Para localizar a posição exata usamos o comprimento do hud. */
            CtxTick ctx_tick;
            if (estado->modo_desafio) {
                /* Monta string igual à do HUD para medir comprimento */
                char hud_tmp[160];
                snprintf(hud_tmp, sizeof(hud_tmp),
                         "  Fase %d/%d  |  Palavras: %d/%d  |  Pontos: %-5d  |  Jogador: %.18s  ",
                         fase_idx + 1, estado->qtd_fases,
                         fase->total - restantes, fase->total,
                         estado->pontos,
                         estado->nome_jogador);
                ctx_tick.estado            = estado;
                ctx_tick.fase_idx          = fase_idx;
                ctx_tick.palavras_restantes = restantes;
                ctx_tick.tempo_total       = tempo_fase;
                ctx_tick.row_hud           = 1; /* HUD fica na 2ª linha (idx 1) */
                ctx_tick.col_timer         = (int)strlen(hud_tmp);
            }

            /* Calcula tempo restante para o timeout */
            int seg_para_timeout = 0;
            if (estado->modo_desafio) {
                int decorrido = (int)(time(NULL) - fase_inicio);
                seg_para_timeout = tempo_fase - decorrido;
                if (seg_para_timeout <= 0) seg_para_timeout = 1; /* pelo menos 1 s */
            }

            int res = ui_ler_linha_timeout(
                tentativa, MAX_PALAVRA - 1,
                seg_para_timeout,
                estado->modo_desafio ? tick_hud : NULL,
                estado->modo_desafio ? (void *)&ctx_tick : NULL
            );

            if (res == 0) {
                /* EOF — salva e encerra */
                ranking_salvar(arq_rank, estado->nome_jogador, estado->pontos);
                return;
            }

            if (res == 2) {
                /* Timeout da fase */
                attron(COLOR_PAIR(PAR_VERMELHO) | A_BOLD);
                printw("\n");
                ui_centralizado(largura, "TEMPO ESGOTADO! Avancando para proxima fase...\n");
                attroff(COLOR_PAIR(PAR_VERMELHO) | A_BOLD);
                refresh();
                ui_pausar(largura);
                goto proxima_fase;
            }

            if (tentativa[0] == '\0') continue; /* linha vazia */

            /* Converte para maiúsculas */
            for (int k = 0; tentativa[k]; k++)
                tentativa[k] = (char)toupper((unsigned char)tentativa[k]);

            if (strcmp(tentativa, "SAIR") == 0) {
                ranking_salvar(arq_rank, estado->nome_jogador, estado->pontos);
                return;
            }

            /* ── Verifica contra as respostas da fase ─────────────────── */
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

            /* ── Validação de dicionário (só para tentativas erradas) ─── */
            if (!acertou && estado->usar_dicionario &&
                !dict_existe(tentativa)) {
                printw("\n");
                attron(COLOR_PAIR(PAR_AMARELO) | A_BOLD);
                ui_centralizado(largura, "\"");
                printw("%s", tentativa);
                printw("\" nao e uma palavra conhecida. Tente outra!\n");
                attroff(COLOR_PAIR(PAR_AMARELO) | A_BOLD);
                refresh();
                ui_pausar(largura);
                continue; /* sem penalidade */
            }

            if (acertou) {
                estado->pontos += PTS_ACERTO;

                /* Bônus por velocidade (modo desafio) */
                int bonus = 0;
                if (estado->modo_desafio) {
                    int decorrido = (int)(time(NULL) - fase_inicio);
                    int restantes_tempo = tempo_fase - decorrido;
                    bonus = calcular_bonus(restantes_tempo, tempo_fase);
                    estado->pontos += bonus;
                }

                printw("\n");
                attron(COLOR_PAIR(PAR_VERDE) | A_BOLD);
                if (bonus > 0) {
                    char msg_bonus[80];
                    snprintf(msg_bonus, sizeof(msg_bonus),
                             "BOA! +%d pts (acerto) + %d pts (bonus velocidade)!\n",
                             PTS_ACERTO, bonus);
                    ui_centralizado(largura, msg_bonus);
                } else {
                    ui_centralizado(largura, "BOA! +100 pontos!\n");
                }
                attroff(COLOR_PAIR(PAR_VERDE) | A_BOLD);
                refresh();
                ui_pausar(largura);
            } else {
                estado->pontos -= PTS_ERRO;
                if (estado->pontos < 0) estado->pontos = 0;
                printw("\n");
                attron(COLOR_PAIR(PAR_VERMELHO) | A_BOLD);
                ui_centralizado(largura,
                    "Palavra incorreta ou ja encontrada! -10 pontos.\n");
                attroff(COLOR_PAIR(PAR_VERMELHO) | A_BOLD);
                refresh();
                ui_pausar(largura);
            }
        } /* while (restantes > 0) */

        /* Fase concluída normalmente */
        estado->pontos += PTS_FASE;
        clear();
        printw("\n\n\n");
        attron(COLOR_PAIR(PAR_VERDE) | A_BOLD);
        char msg_fase[80];
        snprintf(msg_fase, sizeof(msg_fase),
                 "FASE %d CONCLUIDA! +25 pontos. Total: %d pts\n",
                 fase_idx + 1, estado->pontos);
        ui_centralizado(largura, msg_fase);
        attroff(COLOR_PAIR(PAR_VERDE) | A_BOLD);
        refresh();
        ui_pausar(largura);

proxima_fase:
        fase_idx++;
    } /* while (fase_idx < qtd_fases) */

    /* Zerou o jogo */
    clear();
    printw("\n\n\n");
    attron(COLOR_PAIR(PAR_CIANO) | A_BOLD);
    ui_centralizado(largura, "PARABENS! VOCE ZEROU O WMLETRAS!\n\n");
    char msg_final[60];
    snprintf(msg_final, sizeof(msg_final),
             "Pontuacao final: %d pts\n", estado->pontos);
    ui_centralizado(largura, msg_final);
    attroff(COLOR_PAIR(PAR_CIANO) | A_BOLD);
    refresh();

    ranking_salvar(arq_rank, estado->nome_jogador, estado->pontos);
    ui_pausar(largura);
}
