/**
 * @file ui.h
 * @brief Interface de usuário: limpeza de tela, cores, menus e entrada de texto.
 */
#ifndef UI_H
#define UI_H

#include "tipos.h"

/* ── Terminal ───────────────────────────────────────────────────────────── */

/**
 * @brief Detecta a largura do terminal em colunas.
 * @return Largura em colunas; LARGURA_PADRAO (80) se não for possível detectar.
 */
int ui_largura_terminal(void);

/** @brief Limpa a tela usando sequência ANSI (sem system()). */
void ui_limpar_tela(void);

/** @brief Descarta o restante da linha no buffer de stdin. */
void ui_limpar_buffer(void);

/**
 * @brief Imprime @p texto centralizado na tela.
 * @param largura  Largura da tela em colunas.
 * @param texto    String terminada em '\0'.
 */
void ui_centralizado(int largura, const char *texto);

/**
 * @brief Aguarda o usuário pressionar ENTER.
 * @param largura Largura da tela (para centralizar a mensagem).
 */
void ui_pausar(int largura);

/* ── Menu genérico ──────────────────────────────────────────────────────── */

/**
 * @brief Item de um menu navegável.
 */
typedef struct {
    const char *texto;   /**< Rótulo exibido */
} ItemMenu;

/**
 * @brief Exibe um menu navegável com w/s/f e retorna o índice (base 0) escolhido.
 *
 * O menu redesenha a tela a cada tecla. Ao receber EOF, retorna @p padrao_eof.
 *
 * @param titulo      Título exibido no cabeçalho (pode ser NULL).
 * @param itens       Vetor de ItemMenu.
 * @param n           Número de itens.
 * @param padrao_eof  Índice (base 0) a retornar em caso de EOF.
 * @param largura     Largura da tela em colunas.
 * @return Índice (base 0) do item confirmado.
 */
int ui_menu(const char *titulo, const ItemMenu *itens, int n,
            int padrao_eof, int largura);

/* ── Entrada de texto ───────────────────────────────────────────────────── */

/**
 * @brief Lê uma linha de stdin, remove '\n', limita a @p max_chars.
 * @param buf       Buffer de destino.
 * @param max_chars Tamanho máximo a copiar (sem o '\0').
 * @return 1 em sucesso, 0 em EOF.
 */
int ui_ler_linha(char *buf, int max_chars);

/* ── Telas fixas ────────────────────────────────────────────────────────── */

/** @brief Exibe a tela de boas-vindas e coleta o nome do jogador. */
void ui_tela_boas_vindas(EstadoJogo *estado);

/** @brief Exibe as instruções do jogo. */
void ui_instrucoes(int largura);

/** @brief Exibe a tela de encerramento final. */
void ui_tela_fim(int largura);

/** @brief Exibe a tela de introdução animada antes de jogar. */
void ui_tela_inicio_jogo(int largura);

#endif /* UI_H */
