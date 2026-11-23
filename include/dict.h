/**
 * @file dict.h
 * @brief Dicionário embutido de palavras do português para validação de tentativas.
 *
 * Implementação via tabela hash aberta (open addressing, sondagem linear).
 * As palavras são armazenadas em maiúsculas e normalizadas (sem acento).
 */
#ifndef DICT_H
#define DICT_H

/**
 * @brief Verifica se @p palavra (maiúsculas, sem acento) existe no dicionário.
 *
 * A função é case-insensitive internamente: converte @p palavra para maiúsculas
 * antes de consultar.
 *
 * @param palavra  String terminada em '\0'.
 * @return 1 se a palavra está no dicionário, 0 caso contrário.
 */
int dict_existe(const char *palavra);

#endif /* DICT_H */
