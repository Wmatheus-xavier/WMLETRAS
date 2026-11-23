/**
 * @file fases.c
 * @brief Carregamento e validação das fases do WMLETRAS a partir de entrada.txt.
 */
#include "fases.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

/**
 * @brief Verifica se @p palavra pode ser formada pelas letras de @p letras.
 *
 * Conta a frequência de cada letra em @p letras e verifica se @p palavra
 * não usa nenhuma letra mais vezes do que ela aparece em @p letras.
 *
 * @return 1 se válida, 0 caso contrário.
 */
static int resposta_valida(const char *letras, const char *palavra) {
    int freq[26] = {0};
    for (int i = 0; letras[i]; i++) {
        int idx = letras[i] - 'A';
        if (idx >= 0 && idx < 26) freq[idx]++;
    }
    for (int i = 0; palavra[i]; i++) {
        int idx = palavra[i] - 'A';
        if (idx < 0 || idx >= 26) return 0; /* char inválido */
        if (--freq[idx] < 0)      return 0; /* letra insuficiente */
    }
    return 1;
}

int fases_carregar(const char *caminho, EstadoJogo *estado) {
    FILE *arq = fopen(caminho, "r");
    if (!arq) return 0;

    int i = 0;
    char token[MAX_PALAVRA];

    while (i < MAX_FASES && fscanf(arq, "%49s", token) == 1) {
        /* Comentário (#) ou separador (-) — ignora */
        if (token[0] == '#' || (token[0] == '-' && token[1] == '\0')) {
            /* Descarta o restante da linha para não confundir com números */
            int ch;
            while ((ch = fgetc(arq)) != '\n' && ch != EOF)
                ;
            continue;
        }

        /* Copia letras disponíveis para a fase */
        strncpy(estado->fases[i].letras, token, MAX_LETRAS - 1);
        estado->fases[i].letras[MAX_LETRAS - 1] = '\0';

        /* Converte para maiúsculas */
        for (int k = 0; estado->fases[i].letras[k]; k++)
            estado->fases[i].letras[k] =
                (char)toupper((unsigned char)estado->fases[i].letras[k]);

        /* Número de respostas */
        int total;
        if (fscanf(arq, "%d", &total) != 1) break;
        if (total < 0)        total = 0;
        if (total > MAX_RESPOSTAS) total = MAX_RESPOSTAS;
        estado->fases[i].total = total;

        /* Respostas */
        int validas = 0;
        for (int j = 0; j < total; j++) {
            char resp_tmp[MAX_PALAVRA];
            if (fscanf(arq, "%49s", resp_tmp) != 1) break;

            /* Converte para maiúsculas */
            for (int k = 0; resp_tmp[k]; k++)
                resp_tmp[k] = (char)toupper((unsigned char)resp_tmp[k]);

            /* Valida: a resposta deve ser formável com as letras da fase */
            if (!resposta_valida(estado->fases[i].letras, resp_tmp)) {
                fprintf(stderr,
                    "[AVISO] Fase %d: resposta '%s' nao pode ser formada "
                    "pelas letras '%s'. Ignorada.\n",
                    i + 1, resp_tmp, estado->fases[i].letras);
                continue;
            }

            strncpy(estado->fases[i].respostas[validas], resp_tmp, MAX_PALAVRA - 1);
            estado->fases[i].respostas[validas][MAX_PALAVRA - 1] = '\0';
            estado->fases[i].acertos[validas] = 0;
            validas++;
        }
        estado->fases[i].total = validas;

        i++;
    }

    fclose(arq);
    estado->qtd_fases = i;
    return i;
}

void fases_reiniciar_acertos(EstadoJogo *estado) {
    for (int i = 0; i < estado->qtd_fases; i++)
        for (int j = 0; j < estado->fases[i].total; j++)
            estado->fases[i].acertos[j] = 0;
}
