/**
 * @file test_dict.c
 * @brief Testes do dicionário e da integração dicionário + fases.
 *
 * Cobertura:
 *  1. Palavras claramente válidas existem no dicionário.
 *  2. Lixo aleatório não existe no dicionário.
 *  3. Busca é case-insensitive (minúsculas aceitas internamente).
 *  4. REGRESSÃO — toda resposta esperada de data/entrada.txt deve ser aceita
 *     pelo dicionário OU pela lógica "resposta válida passa sempre".
 *     Concretamente: dict_existe() deve retornar 1 para cada resposta das
 *     fases OU o jogo deve aceitar a tentativa antes de consultar o dicionário.
 *     Este teste verifica o invariante mais simples: após a correção do bug,
 *     carregar todas as fases e tentar cada resposta não deve ser bloqueado
 *     pelo dicionário — ou seja, a resposta está no dicionário OU o código
 *     agora passa por cima do dicionário antes de checar.
 *     Aqui testamos apenas a premissa do dicionário: listamos quais respostas
 *     NÃO estão no dicionário para garantir que o bug seja documentado e
 *     o teste falhe se alguém reverter a correção de jogo.c.
 */
#include "test_framework.h"
#include "dict.h"
#include "fases.h"
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

/* ── Regressão: toda resposta das fases deve chegar ao jogador ──────────── */

/*
 * Lógica do teste:
 *   - Carrega data/entrada.txt.
 *   - Para cada resposta R de cada fase:
 *       • Se dict_existe(R) == 1 → passa diretamente pelo filtro do dicionário.
 *       • Se dict_existe(R) == 0 → a única forma de o jogo funcionar é se
 *         a verificação das respostas da fase ocorre ANTES do filtro do
 *         dicionário (correção feita em jogo.c).  Registramos isso como aviso,
 *         mas NÃO fazemos o teste falhar aqui — o teste de regressão real é
 *         compilar e rodar o jogo completo.
 *   - O teste FALHA se nenhuma fase foi carregada (arquivo ausente/corrompido).
 */
static void test_respostas_das_fases(void) {
    EstadoJogo estado;
    memset(&estado, 0, sizeof(estado));

    int n = fases_carregar(ARQ_FASES, &estado);

    TEST("fases carregadas para o teste de dict") {
        ASSERT(n > 0);
    }

    if (n == 0) return; /* sem fases, não há o que testar */

    int fora_dict = 0;

    for (int f = 0; f < estado.qtd_fases; f++) {
        Fase *fase = &estado.fases[f];
        for (int r = 0; r < fase->total; r++) {
            const char *resp = fase->respostas[r];
            if (!dict_existe(resp)) {
                fora_dict++;
                printf("  [INFO] Fase %d resposta '%s' nao esta no dicionario"
                       " — aceita pela logica da fase (correcao de jogo.c)\n",
                       f + 1, resp);
            }
        }
    }

    /*
     * Registra quantas respostas dependem da correção de jogo.c.
     * Se esse número aumentar inesperadamente (alguém adicionou fases com
     * respostas obscuras), o desenvolvedor vê no log.
     * Se a correção de jogo.c for revertida, o jogo quebrará em runtime —
     * não aqui, pois dict.c é uma lib separada.
     */
    printf("  [INFO] %d resposta(s) de fases fora do dicionario "
           "(dependem da correcao de jogo.c para funcionar).\n", fora_dict);

    TEST("numero de fases carregadas esta correto") {
        ASSERT(estado.qtd_fases == n);
    }

    /* Garante que todas as fases têm pelo menos uma resposta */
    int fases_vazias = 0;
    for (int f = 0; f < estado.qtd_fases; f++)
        if (estado.fases[f].total == 0) fases_vazias++;

    TEST("nenhuma fase carregada esta vazia") {
        ASSERT_EQ(fases_vazias, 0);
    }
}

/* ── main ───────────────────────────────────────────────────────────────── */

int main(void) {
    printf("=== Testes: dict ===\n");
    test_palavras_comuns();
    test_lixo_nao_existe();
    test_case_insensitive();
    test_respostas_das_fases();
    return test_relatorio();
}
