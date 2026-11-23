/**
 * @file test_fases.c
 * @brief Testes unitários para o módulo de carregamento de fases.
 */
#include "test_framework.h"
#include "fases.h"
#include "tipos.h"

#include <string.h>
#include <stdio.h>

/* ── Helpers ────────────────────────────────────────────────────────────── */

/** Cria um arquivo temporário com o conteúdo fornecido e retorna o caminho. */
static const char *criar_arquivo_temp(const char *conteudo) {
    static const char *caminho = "/tmp/wmletras_test_entrada.txt";
    FILE *f = fopen(caminho, "w");
    if (f) { fputs(conteudo, f); fclose(f); }
    return caminho;
}

/* ── Testes ─────────────────────────────────────────────────────────────── */

static void test_carregar_fases_basico(void) {
    const char *conteudo =
        "RAAUG\n5\nRUGA\nGRAU\nRUA\nAGUA\nAGUAR\n-\n"
        "IALMT\n3\nLATIM\nLIMA\nTIL\n-\n";
    const char *arq = criar_arquivo_temp(conteudo);

    EstadoJogo estado;
    memset(&estado, 0, sizeof(estado));

    TEST("carregar_fases: retorna numero correto de fases") {
        int n = fases_carregar(arq, &estado);
        ASSERT_EQ(n, 2);
    }

    TEST("carregar_fases: letras da fase 1 corretas") {
        ASSERT_STR_EQ(estado.fases[0].letras, "RAAUG");
    }

    TEST("carregar_fases: total de respostas da fase 1") {
        ASSERT_EQ(estado.fases[0].total, 5);
    }

    TEST("carregar_fases: primeira resposta da fase 1") {
        ASSERT_STR_EQ(estado.fases[0].respostas[0], "RUGA");
    }

    TEST("carregar_fases: letras da fase 2 corretas") {
        ASSERT_STR_EQ(estado.fases[1].letras, "IALMT");
    }

    TEST("carregar_fases: total de respostas da fase 2") {
        ASSERT_EQ(estado.fases[1].total, 3);
    }
}

static void test_carregar_fases_arquivo_inexistente(void) {
    EstadoJogo estado;
    memset(&estado, 0, sizeof(estado));

    TEST("carregar_fases: arquivo inexistente retorna 0") {
        int n = fases_carregar("/nao/existe/entrada.txt", &estado);
        ASSERT_EQ(n, 0);
    }
}

static void test_carregar_fases_resposta_invalida(void) {
    /* "XYZ" não pode ser formada com as letras "RAAUG" */
    const char *conteudo = "RAAUG\n2\nRUGA\nXYZ\n-\n";
    const char *arq = criar_arquivo_temp(conteudo);

    EstadoJogo estado;
    memset(&estado, 0, sizeof(estado));
    fases_carregar(arq, &estado);

    TEST("carregar_fases: resposta invalida e ignorada") {
        ASSERT_EQ(estado.fases[0].total, 1); /* só RUGA é válida */
    }
}

static void test_carregar_fases_converte_maiusculas(void) {
    const char *conteudo = "raaug\n1\nruga\n-\n";
    const char *arq = criar_arquivo_temp(conteudo);

    EstadoJogo estado;
    memset(&estado, 0, sizeof(estado));
    fases_carregar(arq, &estado);

    TEST("carregar_fases: letras convertidas para maiusculas") {
        ASSERT_STR_EQ(estado.fases[0].letras, "RAAUG");
    }
    TEST("carregar_fases: respostas convertidas para maiusculas") {
        ASSERT_STR_EQ(estado.fases[0].respostas[0], "RUGA");
    }
}

static void test_carregar_fases_limite_max(void) {
    /* Fase com total_respostas > MAX_RESPOSTAS deve ser truncada */
    char conteudo[2048] = "ABCDE\n25\n";
    /* Palavras válidas (todas formadas pelas letras ABCDE) */
    const char *palavras[] = {"A","B","C","D","E","AB","AC","AD","AE","BC",
                               "BD","BE","CD","CE","DE","ABC","ABD","ABE",
                               "ACD","ACE","ADE","BCD","BCE","BDE","CDE"};
    for (int i = 0; i < 25; i++) {
        strncat(conteudo, palavras[i], sizeof(conteudo) - strlen(conteudo) - 1);
        strncat(conteudo, "\n",        sizeof(conteudo) - strlen(conteudo) - 1);
    }
    strncat(conteudo, "-\n", sizeof(conteudo) - strlen(conteudo) - 1);

    const char *arq = criar_arquivo_temp(conteudo);
    EstadoJogo estado;
    memset(&estado, 0, sizeof(estado));
    fases_carregar(arq, &estado);

    TEST("carregar_fases: total_respostas truncado a MAX_RESPOSTAS") {
        ASSERT(estado.fases[0].total <= MAX_RESPOSTAS);
    }
}

static void test_reiniciar_acertos(void) {
    EstadoJogo estado;
    memset(&estado, 0, sizeof(estado));
    estado.qtd_fases = 1;
    estado.fases[0].total = 3;
    estado.fases[0].acertos[0] = 1;
    estado.fases[0].acertos[1] = 1;
    estado.fases[0].acertos[2] = 0;

    fases_reiniciar_acertos(&estado);

    TEST("reiniciar_acertos: todos os acertos voltam a 0") {
        ASSERT_EQ(estado.fases[0].acertos[0], 0);
        ASSERT_EQ(estado.fases[0].acertos[1], 0);
        ASSERT_EQ(estado.fases[0].acertos[2], 0);
    }
}

/* ── main ───────────────────────────────────────────────────────────────── */

int main(void) {
    printf("=== Testes: fases ===\n");
    test_carregar_fases_basico();
    test_carregar_fases_arquivo_inexistente();
    test_carregar_fases_resposta_invalida();
    test_carregar_fases_converte_maiusculas();
    test_carregar_fases_limite_max();
    test_reiniciar_acertos();
    return test_relatorio();
}
