/**
 * @file ranking.h
 * @brief Leitura, escrita e exibição do ranking de jogadores.
 */
#ifndef RANKING_H
#define RANKING_H

#include "tipos.h"

/**
 * @brief Salva a pontuação do jogador no arquivo de ranking.
 *
 * Não salva pontuações iguais a 0 (partidas sem nenhum acerto).
 * O arquivo é lido, a nova entrada é adicionada e reescrito
 * completamente, limitado a MAX_RANKING registros.
 *
 * @param caminho  Caminho do arquivo (ex: "ranking.bin").
 * @param nome     Nome do jogador (máx MAX_NOME-1 chars).
 * @param pontos   Pontuação a salvar.
 */
void ranking_salvar(const char *caminho, const char *nome, int pontos);

/**
 * @brief Exibe o ranking na tela.
 *
 * Mostra no máximo 10 entradas, ordenadas do maior para o menor.
 *
 * @param caminho  Caminho do arquivo de ranking.
 * @param largura  Largura do terminal para centralização.
 */
void ranking_exibir(const char *caminho, int largura);

/**
 * @brief Apaga o arquivo de ranking (zera tudo).
 * @param caminho  Caminho do arquivo de ranking.
 */
void ranking_apagar(const char *caminho);

#endif /* RANKING_H */
