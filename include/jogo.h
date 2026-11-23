/**
 * @file jogo.h
 * @brief Lógica principal da partida.
 */
#ifndef JOGO_H
#define JOGO_H

#include "tipos.h"

/**
 * @brief Executa uma partida completa do WMLETRAS.
 *
 * Carrega as fases, zera pontuação, exibe a tela de introdução e entra
 * no loop fase-a-fase. Ao final (SAIR ou término), salva no ranking.
 *
 * @param estado    Ponteiro para o EstadoJogo (nome do jogador já preenchido).
 * @param arq_fases Caminho para o arquivo de fases.
 * @param arq_rank  Caminho para o arquivo de ranking.
 */
void jogo_jogar(EstadoJogo *estado,
                const char *arq_fases,
                const char *arq_rank);

/* ── Resultado de avaliar_tentativa() ─────────────────────────────────────
 *
 *  TENT_ACERTO   — a tentativa bateu com uma resposta pendente da fase.
 *  TENT_DICT     — a tentativa não está no dicionário (bloqueada sem penalidade).
 *  TENT_ERRADA   — palavra real (ou dicionário desligado) mas não é resposta da fase.
 */
typedef enum {
    TENT_ACERTO = 0,
    TENT_DICT   = 1,
    TENT_ERRADA = 2
} ResultadoTentativa;

/**
 * @brief Avalia @p tentativa contra a fase e o dicionário.
 *
 * A tentativa deve estar em maiúsculas.
 * Ordem de verificação:
 *  1. Compara com as respostas pendentes da fase.
 *     Se acertar, marca o acerto em fase->acertos[] e retorna TENT_ACERTO.
 *  2. Se @p usar_dicionario e a palavra não existir no dicionário,
 *     retorna TENT_DICT (sem penalidade, sem marcar acerto).
 *  3. Caso contrário, retorna TENT_ERRADA (penalidade de pontos cabe ao chamador).
 *
 * @param fase           Fase atual (modificada em caso de acerto).
 * @param tentativa      Palavra em maiúsculas, terminada em '\0'.
 * @param usar_dicionario 1 = filtro de dicionário ativo, 0 = desligado.
 */
ResultadoTentativa avaliar_tentativa(Fase       *fase,
                                     const char *tentativa,
                                     int         usar_dicionario);

#endif /* JOGO_H */
