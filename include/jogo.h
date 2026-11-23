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

#endif /* JOGO_H */
