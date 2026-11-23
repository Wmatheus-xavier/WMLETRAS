/**
 * @file test_dict.c
 * @brief Testes do dicionário e da integração dicionário + fases.
 *
 * Cobertura:
 *  1. Palavras confirmadas existem no dicionário.
 *  2. Lixo aleatório não existe no dicionário.
 *  3. Busca é case-insensitive (minúsculas aceitas internamente).
 *  4. REGRESSÃO — toda resposta de data/entrada.txt deve ser aceita por
 *     avaliar_tentativa() com usar_dicionario=1.  O teste chama a função
 *     pura diretamente: se alguém reverter a correção de jogo.c (voltando
 *     a checar o dicionário antes da resposta da fase), este teste FALHA.
 */
#include "test_framework.h"
#include "dict.h"
#include "fases.h"
#include "jogo.h"
#include "tipos.h"

#include <stdio.h>
#include <string.h>

/* Caminho relativo ao diretório raiz do projeto (onde make test é executado) */
#define ARQ_FASES "data/entrada.txt"

/* ── Testes básicos do dicionário ───────────────────────────────────────── */

static void test_palavras_comuns(void) {
    TEST("dict: AMOR existe no dicionario")  { ASSERT(dict_existe("AMOR")); }
    TEST("dict: CARO existe no dicionario")  { ASSERT(dict_existe("CARO")); }
    TEST("dict: CURA existe no dicionario")  { ASSERT(dict_existe("CURA")); }
    TEST("dict: LIMA existe no dicionario")  { ASSERT(dict_existe("LIMA")); }
    TEST("dict: VALE existe no dicionario")  { ASSERT(dict_existe("VALE")); }
    TEST("dict: VELA existe no dicionario")  { ASSERT(dict_existe("VELA")); }
    TEST("dict: CELA existe no dicionario")  { ASSERT(dict_existe("CELA")); }
    TEST("dict: CERO existe no dicionario")  { ASSERT(dict_existe("CERO")); }
    TEST("dict: RICO existe no dicionario")  { ASSERT(dict_existe("RICO")); }
    TEST("dict: CRIA existe no dicionario")  { ASSERT(dict_existe("CRIA")); }
    TEST("dict: FALA existe no dicionario")  { ASSERT(dict_existe("FALA")); }
    TEST("dict: ERRO existe no dicionario")  { ASSERT(dict_existe("ERRO")); }
    TEST("dict: ROBO existe no dicionario")  { ASSERT(dict_existe("ROBO")); }
    TEST("dict: ROCA existe no dicionario")  { ASSERT(dict_existe("ROCA")); }
}

static void test_lixo_nao_existe(void) {
    TEST("dict: XQZWK nao existe") { ASSERT(!dict_existe("XQZWK")); }
    TEST("dict: AAAAA nao existe") { ASSERT(!dict_existe("AAAAA")); }
    TEST("dict: ZZZZ  nao existe") { ASSERT(!dict_existe("ZZZZ"));  }
    TEST("dict: string vazia")     { ASSERT(!dict_existe(""));       }
}

static void test_case_insensitive(void) {
    TEST("dict: 'amor' (minusculo) aceito") { ASSERT(dict_existe("amor")); }
    TEST("dict: 'Amor' (misto)    aceito") { ASSERT(dict_existe("Amor")); }
    TEST("dict: 'cRiA' (misto)    aceito") { ASSERT(dict_existe("cRiA")); }
}

/* ── Regressão: toda resposta das fases deve ser aceita com dicionário ativo */

/*
 * Chama avaliar_tentativa() — a função pura extraída de jogo.c — com
 * usar_dicionario=1 para cada resposta de cada fase.
 *
 * Invariante: avaliar_tentativa() DEVE retornar TENT_ACERTO para qualquer
 * resposta válida da fase, independentemente de o dicionário a conter ou não,
 * porque a verificação das respostas ocorre ANTES do filtro do dicionário.
 *
 * Se a correção em jogo.c for revertida (filtro antes da resposta), o
 * resultado será TENT_DICT para as ~38 respostas ausentes do dicionário,
 * e este teste falhará imediatamente.
 */
static void test_respostas_aceitas_com_dict_ativo(void) {
    EstadoJogo estado;
    memset(&estado, 0, sizeof(estado));

    int n = fases_carregar(ARQ_FASES, &estado);

    TEST("fases carregadas para teste de regressao") {
        ASSERT(n > 0);
    }

    if (n == 0) return;

    for (int f = 0; f < estado.qtd_fases; f++) {
        /*
         * Para cada resposta criamos uma cópia temporária da fase com todos
         * os acertos zerados, garantindo que a resposta ainda está "pendente"
         * no momento da chamada (avaliar_tentativa marca fase->acertos[j] = 1
         * ao acertar, então reutilizar a mesma fase saltaria respostas já
         * marcadas).
         */
        for (int r = 0; r < estado.fases[f].total; r++) {
            Fase copia = estado.fases[f];
            memset(copia.acertos, 0, sizeof(copia.acertos));

            const char *resp = copia.respostas[r];

            char nome_teste[128];
            snprintf(nome_teste, sizeof(nome_teste),
                     "regressao: fase %d resposta '%s' aceita (dict ativo)",
                     f + 1, resp);

            ResultadoTentativa resultado =
                avaliar_tentativa(&copia, resp, /*usar_dicionario=*/1);

            TEST(nome_teste) {
                ASSERT_EQ(resultado, TENT_ACERTO);
            }
        }
    }
}

/* ── main ───────────────────────────────────────────────────────────────── */

int main(void) {
    printf("=== Testes: dict ===\n");
    test_palavras_comuns();
    test_lixo_nao_existe();
    test_case_insensitive();
    test_respostas_aceitas_com_dict_ativo();
    return test_relatorio();
}
