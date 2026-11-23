/**
 * @file ranking.c
 * @brief Persistência e exibição do ranking de jogadores do WMLETRAS.
 *
 * Formato do arquivo ranking.bin (portátil):
 *   - 4 bytes: magic number (0x574D4C52 = "WMLR")
 *   - 4 bytes: versão (int, little-endian escrito campo a campo)
 *   - 4 bytes: quantidade de entradas
 *   - N * (MAX_NOME + 4) bytes: nome (MAX_NOME chars) + pontos (int 4 bytes)
 *
 * Escrevemos campo a campo com fwrite de tipos primitivos para evitar problemas
 * de padding entre compiladores e arquiteturas.
 */
#include "ranking.h"

#include <stdio.h>
#include <string.h>

#define ARQUIVO_RANKING "ranking.bin"
#define TOP_EXIBIR      10

/* ── Helpers de I/O portátil ─────────────────────────────────────────────── */

static int escrever_int(FILE *f, int v) {
    /* Escreve 4 bytes em little-endian explícito */
    unsigned char b[4];
    b[0] = (unsigned char)( v        & 0xFF);
    b[1] = (unsigned char)((v >>  8) & 0xFF);
    b[2] = (unsigned char)((v >> 16) & 0xFF);
    b[3] = (unsigned char)((v >> 24) & 0xFF);
    return (int)fwrite(b, 1, 4, f) == 4 ? 1 : 0;
}

static int ler_int(FILE *f, int *v) {
    unsigned char b[4];
    if (fread(b, 1, 4, f) != 4) return 0;
    *v = (int)((unsigned)b[0]
             | ((unsigned)b[1] <<  8)
             | ((unsigned)b[2] << 16)
             | ((unsigned)b[3] << 24));
    return 1;
}

/* ── Leitura do arquivo ──────────────────────────────────────────────────── */

static int ranking_ler(const char *caminho, ArquivoRanking *out) {
    FILE *f = fopen(caminho, "rb");
    if (!f) {
        out->qtd = 0;
        return 0;
    }

    int magia, versao, qtd;
    if (!ler_int(f, &magia) || (unsigned)magia != RANKING_MAGIA ||
        !ler_int(f, &versao) || versao != RANKING_VERSAO ||
        !ler_int(f, &qtd)) {
        fclose(f);
        out->qtd = 0;
        return 0;
    }
    if (qtd < 0 || qtd > MAX_RANKING) qtd = 0;
    out->qtd = qtd;

    for (int i = 0; i < qtd; i++) {
        if (fread(out->entradas[i].nome, 1, MAX_NOME, f) != (size_t)MAX_NOME ||
            !ler_int(f, &out->entradas[i].pontos)) {
            out->qtd = i; /* trunca ao que foi lido */
            break;
        }
        /* Garante terminação mesmo com arquivo corrompido */
        out->entradas[i].nome[MAX_NOME - 1] = '\0';
    }

    fclose(f);
    return 1;
}

/* ── Escrita do arquivo ──────────────────────────────────────────────────── */

static int ranking_escrever(const char *caminho, const ArquivoRanking *r) {
    FILE *f = fopen(caminho, "wb");
    if (!f) return 0;

    escrever_int(f, (int)RANKING_MAGIA);
    escrever_int(f, RANKING_VERSAO);
    escrever_int(f, r->qtd);

    for (int i = 0; i < r->qtd; i++) {
        char nome_padded[MAX_NOME];
        memset(nome_padded, 0, MAX_NOME);
        strncpy(nome_padded, r->entradas[i].nome, MAX_NOME - 1);
        fwrite(nome_padded, 1, MAX_NOME, f);
        escrever_int(f, r->entradas[i].pontos);
    }

    fclose(f);
    return 1;
}

/* ── Ordenação (bubble sort — adequado para MAX_RANKING = 100) ──────────── */

static void ranking_ordenar(ArquivoRanking *r) {
    for (int i = 0; i < r->qtd - 1; i++) {
        for (int j = 0; j < r->qtd - i - 1; j++) {
            if (r->entradas[j].pontos < r->entradas[j + 1].pontos) {
                EntradaRanking tmp  = r->entradas[j];
                r->entradas[j]     = r->entradas[j + 1];
                r->entradas[j + 1] = tmp;
            }
        }
    }
}

/* ── API pública ─────────────────────────────────────────────────────────── */

void ranking_salvar(const char *caminho, const char *nome, int pontos) {
    /* Não salva partidas sem nenhum ponto */
    if (pontos <= 0) return;

    ArquivoRanking r;
    ranking_ler(caminho, &r);

    if (r.qtd < MAX_RANKING) {
        strncpy(r.entradas[r.qtd].nome, nome, MAX_NOME - 1);
        r.entradas[r.qtd].nome[MAX_NOME - 1] = '\0';
        r.entradas[r.qtd].pontos = pontos;
        r.qtd++;
    }

    ranking_ordenar(&r);
    ranking_escrever(caminho, &r);
}

void ranking_exibir(const char *caminho, int largura) {
    ArquivoRanking r;
    int ok = ranking_ler(caminho, &r);

    printf("\033[2J\033[H"); /* limpa tela */
    printf("%s", COR_VERDE);

    int marg = (largura - 37) / 2;
    if (marg < 0) marg = 0;

    printf("%*s%s\n", marg, "", "+-----------------------------------+");
    printf("%*s%s\n", marg, "", "|      RANKING  (TOP 10)            |");
    printf("%*s%s\n", marg, "", "+-----------------------------------+");

    if (!ok || r.qtd == 0) {
        printf("%*s%s\n", marg, "", "|  Nenhum registro encontrado.      |");
    } else {
        int exibir = r.qtd < TOP_EXIBIR ? r.qtd : TOP_EXIBIR;
        for (int i = 0; i < exibir; i++) {
            /* Trunca nome a 15 chars para garantir que a linha caiba na caixa */
            char linha[48];
            snprintf(linha, sizeof(linha), "| %2d. %-15.15s %8d pts |",
                     i + 1,
                     r.entradas[i].nome,
                     r.entradas[i].pontos);
            printf("%*s%s\n", marg, "", linha);
        }
    }

    printf("%*s%s\n", marg, "", "+-----------------------------------+");
    printf("%s", COR_RESET);
}

void ranking_apagar(const char *caminho) {
    /* Recria o arquivo com cabeçalho válido e qtd = 0 */
    ArquivoRanking r;
    r.magia   = RANKING_MAGIA;
    r.versao  = RANKING_VERSAO;
    r.qtd     = 0;
    ranking_escrever(caminho, &r);
}
