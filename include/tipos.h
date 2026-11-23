/**
 * @file tipos.h
 * @brief Tipos e constantes compartilhados entre todos os módulos do WMLETRAS.
 */
#ifndef TIPOS_H
#define TIPOS_H

/* ── Limites ────────────────────────────────────────────────────────────── */
#define MAX_FASES          50
#define MAX_RESPOSTAS      20
#define MAX_LETRAS         50
#define MAX_PALAVRA        50
#define MAX_NOME           100
#define LARGURA_PADRAO     80   /**< Fallback quando não é possível detectar o terminal */

/* ── Pontuação ──────────────────────────────────────────────────────────── */
#define PTS_ACERTO        100
#define PTS_ERRO           10   /**< Valor descontado por erro (subtrai, nunca fica negativo) */
#define PTS_FASE           25

/* ── Modo desafio — cronômetro ──────────────────────────────────────────── */
#define TEMPO_PADRAO_FASE  60   /**< Segundos por fase no modo desafio (padrão) */
#define BONUS_RAPIDO_MAX   50   /**< Bônus máximo por acerto rápido             */
#define BONUS_RAPIDO_LIMIAR 10  /**< Segundos restantes abaixo do qual o bônus vale BONUS_RAPIDO_MAX */

/* ── Cores ANSI (fallback sem ncurses) ──────────────────────────────────── */
#define COR_VERDE    "\033[1;32m"
#define COR_VERMELHO "\033[1;31m"
#define COR_CIANO    "\033[1;36m"
#define COR_AMARELO  "\033[1;33m"
#define COR_RESET    "\033[0m"

/* ── Struct de uma fase ─────────────────────────────────────────────────── */
typedef struct {
    char letras[MAX_LETRAS];           /**< Letras disponíveis (embaralhadas) */
    int  total;                        /**< Quantidade de respostas válidas   */
    char respostas[MAX_RESPOSTAS][MAX_PALAVRA];
    int  acertos[MAX_RESPOSTAS];       /**< 1 = já encontrada, 0 = pendente  */
} Fase;

/* ── Estado global do jogo (substitui variáveis globais soltas) ─────────── */
typedef struct {
    Fase fases[MAX_FASES];
    int  qtd_fases;
    int  pontos;
    int  bonus_easter_egg;             /**< Pontos do easter egg, aplicados antes do jogo */
    char nome_jogador[MAX_NOME];
    int  largura_tela;                 /**< Detectada em tempo de execução   */

    /* ── Modo desafio ──────────────────────────────────────────────────── */
    int  modo_desafio;                 /**< 1 = modo desafio ativo, 0 = normal */
    int  tempo_por_fase;               /**< Segundos disponíveis por fase    */
    int  usar_dicionario;              /**< 1 = valida palavra no dicionário  */
} EstadoJogo;

/* ── Entrada no ranking ─────────────────────────────────────────────────── */
#define RANKING_MAGIA  0x574D4C52u  /**< "WMLR" — identifica o formato       */
#define RANKING_VERSAO 1
#define MAX_RANKING    100

typedef struct {
    char nome[MAX_NOME];
    int  pontos;
} EntradaRanking;

typedef struct {
    unsigned int magia;
    int          versao;
    int          qtd;
    EntradaRanking entradas[MAX_RANKING];
} ArquivoRanking;

#endif /* TIPOS_H */
