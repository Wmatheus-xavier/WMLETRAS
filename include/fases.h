/**
 * @file fases.h
 * @brief Carregamento e validação das fases a partir de entrada.txt.
 */
#ifndef FASES_H
#define FASES_H

#include "tipos.h"

/**
 * @brief Carrega as fases de @p caminho para o estado do jogo.
 *
 * Respeita os limites MAX_FASES e MAX_RESPOSTAS. Respostas são convertidas
 * para maiúsculas na leitura. Linhas com apenas "-" são separadores e
 * ignoradas.
 *
 * @param caminho  Caminho para o arquivo de fases (ex: "data/entrada.txt").
 * @param estado   Ponteiro para o EstadoJogo a preencher.
 * @return Número de fases carregadas (0 indica falha).
 */
int fases_carregar(const char *caminho, EstadoJogo *estado);

/**
 * @brief Reinicia os acertos de todas as fases para 0.
 *
 * Deve ser chamado ao iniciar uma nova partida para não acumular
 * acertos de partidas anteriores.
 *
 * @param estado Ponteiro para o EstadoJogo.
 */
void fases_reiniciar_acertos(EstadoJogo *estado);

#endif /* FASES_H */
