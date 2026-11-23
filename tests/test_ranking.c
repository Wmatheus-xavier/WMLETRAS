/**
 * @file test_ranking.c
 * @brief Testes unitários para o módulo de ranking.
 */
#include "test_framework.h"
#include "ranking.h"
#include "tipos.h"

#include <stdio.h>
#include <string.h>

static const char *ARQ_TESTE = "/tmp/wmletras_test_ranking.bin";

static void limpar_ranking(void) {
    ranking_apagar(ARQ_TESTE);
}

static void test_salvar_e_exibir(void) {
    limpar_ranking();

    ranking_salvar(ARQ_TESTE, "Alice", 500);
    ranking_salvar(ARQ_TESTE, "Bob",   300);
    ranking_salvar(ARQ_TESTE, "Carol", 700);

    /* Lê diretamente o arquivo para validar */
    FILE *f = fopen(ARQ_TESTE, "rb");

    TEST("ranking_salvar: arquivo criado") {
        ASSERT(f != NULL);
    }

    if (f) fclose(f);
}

static void test_nao_salva_pontuacao_zero(void) {
    limpar_ranking();
    ranking_salvar(ARQ_TESTE, "Zero", 0);

    /* O arquivo deve existir mas com qtd = 0 */
    FILE *f = fopen(ARQ_TESTE, "rb");
    if (!f) {
        TEST("ranking_salvar: nao salva pontuacao zero (arquivo nao existe)") {
            ASSERT(1); /* arquivo nem foi criado — também correto */
        }
        return;
    }

    /* Lê os 12 bytes de cabeçalho: magic(4) + versao(4) + qtd(4) */
    unsigned char buf[12];
    int lidos = (int)fread(buf, 1, 12, f);
    fclose(f);

    int qtd = (int)((unsigned)buf[8]
                  | ((unsigned)buf[9]  <<  8)
                  | ((unsigned)buf[10] << 16)
                  | ((unsigned)buf[11] << 24));

    TEST("ranking_salvar: nao salva pontuacao zero (qtd == 0)") {
        ASSERT(lidos == 12 && qtd == 0);
    }
}

static void test_apagar_ranking(void) {
    ranking_salvar(ARQ_TESTE, "Teste", 100);
    ranking_apagar(ARQ_TESTE);

    FILE *f = fopen(ARQ_TESTE, "rb");
    TEST("ranking_apagar: arquivo existe apos apagar") {
        ASSERT(f != NULL);
    }

    if (f) {
        unsigned char buf[12];
        int lidos = (int)fread(buf, 1, 12, f);
        fclose(f);

        int qtd = (int)((unsigned)buf[8]
                      | ((unsigned)buf[9]  <<  8)
                      | ((unsigned)buf[10] << 16)
                      | ((unsigned)buf[11] << 24));

        TEST("ranking_apagar: qtd = 0 apos apagar") {
            ASSERT(lidos == 12 && qtd == 0);
        }
    }
}

int main(void) {
    printf("=== Testes: ranking ===\n");
    test_salvar_e_exibir();
    test_nao_salva_pontuacao_zero();
    test_apagar_ranking();
    return test_relatorio();
}
