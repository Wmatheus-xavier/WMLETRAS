/**
 * @file ui.h
 * @brief Interface de usuário baseada em ncurses — posicionamento de cursor,
 *        entrada em tempo real (sem ENTER) e cores.
 *
 * Toda a saída usa ncurses internamente.  As funções de limpeza de tela,
 * centralização e menus continuam com a mesma assinatura pública para
 * minimizar mudanças nos outros módulos.
 */
#ifndef UI_H
#define UI_H

#include "tipos.h"

/* ── Pares de cores ncurses ─────────────────────────────────────────────── */
/* Inicializados em ui_iniciar(). Use-os com COLOR_PAIR(). */
#define PAR_VERDE    1   /**< Texto verde brilhante no fundo preto  */
#define PAR_VERMELHO 2   /**< Texto vermelho brilhante no fundo preto */
#define PAR_CIANO    3   /**< Texto ciano brilhante no fundo preto  */
#define PAR_AMARELO  4   /**< Texto amarelo brilhante no fundo preto */
#define PAR_NORMAL   5   /**< Texto branco normal no fundo preto    */

/* ── Ciclo de vida ──────────────────────────────────────────────────────── */

/**
 * @brief Inicializa ncurses, cores e modos de entrada.
 *
 * Deve ser chamada uma única vez antes de qualquer outra função de UI.
 * Configura: cbreak, noecho, keypad, curs_set(0).
 */
void ui_iniciar(void);

/**
 * @brief Finaliza ncurses e restaura o terminal.
 *
 * Deve ser chamada antes de retornar de main().
 */
void ui_encerrar(void);

/* ── Terminal ───────────────────────────────────────────────────────────── */

/**
 * @brief Retorna a largura atual da janela ncurses (COLS).
 */
int ui_largura_terminal(void);

/** @brief Apaga a tela ncurses (clear + refresh). */
void ui_limpar_tela(void);

/** @brief Descarta caracteres pendentes no buffer de entrada (via flushinp). */
void ui_limpar_buffer(void);

/**
 * @brief Imprime @p texto centralizado usando a largura @p largura.
 * @param largura  Largura em colunas.
 * @param texto    String terminada em '\0'.
 */
void ui_centralizado(int largura, const char *texto);

/**
 * @brief Aguarda o usuário pressionar qualquer tecla.
 * @param largura Largura da tela (para centralizar a mensagem).
 */
void ui_pausar(int largura);

/* ── Menu genérico ──────────────────────────────────────────────────────── */

/**
 * @brief Item de um menu navegável.
 */
typedef struct {
    const char *texto;
} ItemMenu;

/**
 * @brief Exibe um menu navegável com setas/w-s/f e retorna o índice (base 0).
 *
 * Entrada em tempo real (sem ENTER para navegar, ENTER ou 'f' para confirmar).
 * Ao receber ERR (EOF/timeout), retorna @p padrao_eof.
 *
 * @param titulo     Título exibido no cabeçalho (pode ser NULL).
 * @param itens      Vetor de ItemMenu.
 * @param n          Número de itens.
 * @param padrao_eof Índice (base 0) a retornar em caso de EOF.
 * @param largura    Largura da tela em colunas.
 * @return Índice (base 0) do item confirmado.
 */
int ui_menu(const char *titulo, const ItemMenu *itens, int n,
            int padrao_eof, int largura);

/* ── Entrada de texto ───────────────────────────────────────────────────── */

/**
 * @brief Lê uma string caractere a caractere (ncurses), exibindo um cursor
 *        na linha atual.  Suporta backspace.  Termina com ENTER.
 *
 * @param buf       Buffer de destino.
 * @param max_chars Tamanho máximo a copiar (sem o '\0').
 * @return 1 em sucesso, 0 em EOF/ERR.
 */
int ui_ler_linha(char *buf, int max_chars);

/**
 * @brief Versão com timeout: igual a ui_ler_linha mas interrompe após
 *        @p timeout_s segundos, retornando 2 (timeout) em vez de 1.
 *
 * Enquanto aguarda, chama @p on_tick(segundos_restantes, dados_tick) a cada
 * segundo para permitir que o chamador atualize o HUD do cronômetro.
 *
 * @param buf          Buffer de destino.
 * @param max_chars    Tamanho máximo a copiar (sem '\0').
 * @param timeout_s    Segundos até o timeout (0 = sem limite, igual a ui_ler_linha).
 * @param on_tick      Callback chamado a cada segundo (pode ser NULL).
 * @param dados_tick   Ponteiro opaco passado ao callback (pode ser NULL).
 * @return 1 = leitura normal, 0 = EOF/ERR, 2 = timeout.
 */
int ui_ler_linha_timeout(char *buf, int max_chars, int timeout_s,
                         void (*on_tick)(int segundos_restantes, void *dados),
                         void *dados_tick);

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
