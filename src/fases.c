/**
 * @file fases.c
 * @brief Carregamento e validação das fases do WMLETRAS a partir de entrada.txt.
 */
#include "fases.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

int fases_carregar(const char *caminho, EstadoJogo *estado) {
    FILE *arq = fopen(caminho, "r");
    if (!arq) return 0;

    int i = 0;
    char token[MAX_PALAVRA];

    while (i < MAX_FASES && fscanf(arq, "%49s", token) == 1) {
        /* Separador de fases — ignora */
        if (token[0] == '-' && token[1] == '\0')
            continue;

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
        for (int j = 0; j < total; j++) {
            if (fscanf(arq, "%49s", estado->fases[i].respostas[j]) != 1) break;
            /* Converte resposta para maiúsculas */
            for (int k = 0; estado->fases[i].respostas[j][k]; k++)
                estado->fases[i].respostas[j][k] =
                    (char)toupper((unsigned char)estado->fases[i].respostas[j][k]);
            estado->fases[i].acertos[j] = 0;
        }

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
