/**
 * @file test_framework.h
 * @brief Micro-framework de testes sem dependências externas.
 *
 * Uso:
 *   TEST("descricao") { ASSERT(expressao); }
 *   return test_relatorio();
 */
#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>
#include <string.h>

static int _testes_total  = 0;
static int _testes_falhos = 0;
static const char *_teste_atual = "";

#define TEST(nome) \
    for (_teste_atual = (nome), _testes_total++, printf("  [ RUN ] %s\n", nome); \
         _teste_atual != NULL; \
         _teste_atual = NULL)

#define ASSERT(expr) \
    do { \
        if (!(expr)) { \
            fprintf(stderr, "  [FAIL] %s\n         Assertion falhou: %s\n" \
                    "         %s:%d\n", _teste_atual, #expr, __FILE__, __LINE__); \
            _testes_falhos++; \
        } else { \
            printf("  [ OK ] %s\n", _teste_atual); \
        } \
    } while (0)

#define ASSERT_EQ(a, b) \
    do { \
        if ((a) != (b)) { \
            fprintf(stderr, "  [FAIL] %s\n         Esperado %d, obtido %d\n" \
                    "         %s:%d\n", _teste_atual, (int)(b), (int)(a), \
                    __FILE__, __LINE__); \
            _testes_falhos++; \
        } else { \
            printf("  [ OK ] %s\n", _teste_atual); \
        } \
    } while (0)

#define ASSERT_STR_EQ(a, b) \
    do { \
        if (strcmp((a), (b)) != 0) { \
            fprintf(stderr, "  [FAIL] %s\n         Esperado '%s', obtido '%s'\n" \
                    "         %s:%d\n", _teste_atual, (b), (a), __FILE__, __LINE__); \
            _testes_falhos++; \
        } else { \
            printf("  [ OK ] %s\n", _teste_atual); \
        } \
    } while (0)

static inline int test_relatorio(void) {
    printf("\n=== Resultado: %d/%d testes passaram",
           _testes_total - _testes_falhos, _testes_total);
    if (_testes_falhos == 0)
        printf(" ✓ ===\n");
    else
        printf(" — %d FALHAS ===\n", _testes_falhos);
    return _testes_falhos > 0 ? 1 : 0;
}

#endif /* TEST_FRAMEWORK_H */
