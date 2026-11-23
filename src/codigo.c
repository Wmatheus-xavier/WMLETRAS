#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define LARGURA_TELA 123
#ifdef _WIN32
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif

//washington matheus almeida xavier

//>>ATENCAO<<
//quando for jogar vc ve o tamanho do seu terminal e coloca no #define LARGURA_TELA
// o arquivo entrada.txt deve estar na mesma pasta do executavel
//fiz um codigo secreto que dar muitos pontos

//pesquisei como abria arquivo porque tava dando erro https://youtu.be/MtYuhRtQPHU?si=JRl_IjuhQ8Fgwdpn
//pesquisei como limpar a tela. https://stackoverflow.com/questions/55672661/what-does-printf-033h-033j-do-in-c
//pesquisei como mudava a cor do terminal para parecer hack https://youtu.be/Hl-7xMl7bn0?si=vyftMrx6yjXdfLHp

// Declaracoes de funcoes (prototipos)

int pontos = 0;
int qtd_fases_carregadas = 0;

typedef struct {
    char nome[100];
    int pontoss;
} SaveGame;

typedef struct {
    char letras_embaralhadas[50];
    int total_respostas;
    char respostas[20][50];
    int acertos[20];
} Fase;
Fase lista_fases[50];

int contartama(char *texto);
void centralizado(char *texto);
void limpar_buffer();
void pausar();
void limparTela();
void imprimiresp(int qtd);
void salvar_progresso(char* nome);
void terminarjogo();
int sairdojogo();
void ordenar_ranking(SaveGame *vetor, int qtd);
void exibir_ranking();
void instrucoes();
int carregar_fases();
int jogar(char nomes[]);
int jogo();
void apagarranking();
int menu();
int configuracao();
void chamafuncoesprincipais(char nomes[]);
void erro_palavra();
void validar_nome(int* y, char nomes[], int* nome_valido);
void menu1(char* nomes);

void centralizado(char *texto) {
    int tam_texto = contartama(texto);
    int espacos = (LARGURA_TELA - tam_texto) / 2;
    if (espacos < 0) {
        espacos = 0;
    }
    printf("%*s%s", espacos, "", texto);
}

// Descarta todos os caracteres pendentes no buffer de entrada ate o '\n' ou EOF
void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Congela a tela ate o usuario pressionar ENTER
void pausar() {
    printf("\n");
    centralizado("Pressione ENTER para continuar...");
    limpar_buffer();
}

// Limpa a tela
void limparTela() {
    system(CLEAR);
}

// Conta o tamanho da string
int contartama(char *texto) {
    int tam = 0;
    while (texto[tam] != '\0') {
        tam++;
    }
    return tam;
}

// Imprime espacos para centralizar manualmente
void imprimiresp(int qtd) {
    for (int i = 0; i < qtd; i++) {
        printf(" ");
    }
}

// Salva pontuacao no arquivo binario
void salvar_progresso(char* nome) {
    SaveGame dados;
    strcpy(dados.nome, nome);
    dados.pontoss = pontos;
    FILE *arq = fopen("ranking.bin", "ab");
    if (arq != NULL) {
        fwrite(&dados, sizeof(SaveGame), 1, arq);
        fclose(arq);
    }
}

// Exibe tela de encerramento
void terminarjogo() {
    limparTela();
    printf("\033[1;32m");
    printf("\n\n\n\n");
    centralizado("Jogo do washington xavier da matricula 252024622\n");
    centralizado(" foi encerrado! nunca e um adeus");
    printf("\n\n\n\n");
    printf("\033[0m");
}

// Pergunta ao usuario se deseja sair do jogo
int sairdojogo() {
    int sel = 1;
    int tot = 2;
    char linha[16];
    char tamanhoooo[] = "-------------------------------------";
    int tu = contartama(tamanhoooo);
    int margem_menuuu = (LARGURA_TELA - tu) / 2;

    while (1) {
        limparTela();
        printf("\033[1;32m");
        printf("\n");
        imprimiresp(margem_menuuu);
        printf("-------------------------------------\n");
        imprimiresp(margem_menuuu);
        printf("      TEM CERTEZA QUE DESEJA         \n");
        imprimiresp(margem_menuuu);
        printf("          SAIR DO JOGO?              \n");
        imprimiresp(margem_menuuu);
        printf("-------------------------------------\n");
        imprimiresp(margem_menuuu);
        printf("                                     \n");
        if (sel == 1) {
            imprimiresp(margem_menuuu);
            printf("              -> Sim <-              \n");
        } else {
            imprimiresp(margem_menuuu);
            printf("                 Sim                 \n");
        }
        if (sel == 2) {
            imprimiresp(margem_menuuu);
            printf("              -> Nao <-              \n");
        } else {
            imprimiresp(margem_menuuu);
            printf("                 Nao                 \n");
        }
        imprimiresp(margem_menuuu);
        printf("                                     \n");
        imprimiresp(margem_menuuu);
        printf("-------------------------------------\n");
        centralizado("COMANDOS: 'w' (sobe), 's' (desce), 'f' (confirma)\n");
        centralizado("Sua escolha: ");

        if (fgets(linha, sizeof(linha), stdin) == NULL) break;
        char comando = linha[0];

        if (comando == 'w' || comando == 'W') {
            sel--;
            if (sel < 1) sel = tot;
        } else if (comando == 's' || comando == 'S') {
            sel++;
            if (sel > tot) sel = 1;
        } else if (comando == 'f' || comando == 'F') {
            if (sel == 1) {
                return 1;
            } else {
                return 0;
            }
        }
    }
    return 0; // EOF — retorna Nao por padrao
}

// Ordena ranking do maior para o menor
void ordenar_ranking(SaveGame *vetor, int qtd) {
    SaveGame aux;
    for (int i = 0; i < qtd - 1; i++) {
        for (int j = 0; j < qtd - i - 1; j++) {
            if (vetor[j].pontoss < vetor[j + 1].pontoss) {
                aux = vetor[j];
                vetor[j] = vetor[j + 1];
                vetor[j + 1] = aux;
            }
        }
    }
}

// Exibe o ranking de jogadores
void exibir_ranking() {
    limparTela();
    printf("\033[1;32m");

    FILE *arq = fopen("ranking.bin", "rb");

    if (arq == NULL) {
        printf("\n\n\n");
        centralizado("AINDA NAO HA NENHUM RANKING SALVO\n");
    } else {
        SaveGame jogadores[100];
        int qtd = 0;
        while (qtd < 100 && fread(&jogadores[qtd], sizeof(SaveGame), 1, arq) == 1) {
            qtd++;
        }
        fclose(arq);
        ordenar_ranking(jogadores, qtd);
        centralizado("-------------------------------------\n");
        centralizado("        RANKING (TOP JOGADORES)      \n");
        centralizado("-------------------------------------\n");
        printf("\n");

        for (int i = 0; i < qtd; i++) {
            printf("%*s%dº - %s .......... %d pts\n",
                   (LARGURA_TELA - 40) / 2, "",
                   i + 1,
                   jogadores[i].nome,
                   jogadores[i].pontoss);
        }
        printf("\n");
        centralizado("-------------------------------------\n");
    }
    pausar();
}

// Exibe as instrucoes do jogo
void instrucoes() {
    char tamanhoooo[] = "-------------------------------------";
    int tu = contartama(tamanhoooo);
    int margem_menuuu = (LARGURA_TELA - tu) / 2;

    limparTela();
    printf("\033[1;32m");
    imprimiresp(margem_menuuu);
    printf("-------------------------------------\n");
    imprimiresp(margem_menuuu);
    printf("        COMO JOGAR WMLETRAS          \n");
    imprimiresp(margem_menuuu);
    printf("-------------------------------------\n");
    imprimiresp(margem_menuuu);
    printf("                                     \n");
    imprimiresp(margem_menuuu);
    printf(" 1. Conecte as letras embaralhadas.  \n");
    imprimiresp(margem_menuuu);
    printf("   2. Forme palavras para pontuar.   \n");
    imprimiresp(margem_menuuu);
    printf("      3. Acerte a palavra para       \n");
    imprimiresp(margem_menuuu);
    printf("          avancar de fase.           \n");
    imprimiresp(margem_menuuu);
    printf(" Acertou a palavra: +100 pontos      \n");
    imprimiresp(margem_menuuu);
    printf(" Errou a palavra:   -10 pontos       \n");
    imprimiresp(margem_menuuu);
    printf(" Passou de fase:    +25 pontos       \n");
    imprimiresp(margem_menuuu);
    printf(" Digite SAIR para salvar e sair.     \n");
    imprimiresp(margem_menuuu);
    printf("                                     \n");
    imprimiresp(margem_menuuu);
    printf("-------------------------------------\n");

    pausar();
}

// Carrega as fases do arquivo entrada.txt
int carregar_fases() {
    FILE *arq = fopen("entrada.txt", "r");

    if (arq == NULL) return 0;
    int i = 0;

    while (i < 50 && fscanf(arq, "%49s", lista_fases[i].letras_embaralhadas) == 1) {
        if (strcmp(lista_fases[i].letras_embaralhadas, "-") == 0) {
            continue;
        }

        if (fscanf(arq, "%d", &lista_fases[i].total_respostas) != 1) break;

        // Limita o total de respostas ao maximo suportado
        if (lista_fases[i].total_respostas > 20) {
            lista_fases[i].total_respostas = 20;
        }

        for (int j = 0; j < lista_fases[i].total_respostas; j++) {
            if (fscanf(arq, "%49s", lista_fases[i].respostas[j]) != 1) break;
            lista_fases[i].acertos[j] = 0;
        }

        i++;
    }
    fclose(arq);

    return i;
}

// Logica principal do jogo
int jogar(char nomes[]) {
    // Zera a pontuacao ao iniciar uma nova partida
    pontos = 0;

    qtd_fases_carregadas = carregar_fases();

    if (qtd_fases_carregadas == 0) {
        centralizado("ERRO: Arquivo 'entrada.txt' vazio ou nao encontrado!\n");
        centralizado("Certifique-se de que 'entrada.txt' esta na pasta do executavel.\n");

        pausar();

        return 1;
    }

    char tentativa[50];
    int fase_atual = 0;

    while (fase_atual < qtd_fases_carregadas) {

        int palavras_restantes = lista_fases[fase_atual].total_respostas;

        while (palavras_restantes > 0) {
            limparTela();
            printf("\033[1;32m");
            printf("\nQuantidade de palavras: %d | Faltam: %d | Pontos: %d\n",
                   lista_fases[fase_atual].total_respostas, palavras_restantes, pontos);
            printf("\n");

            char titulo[50];
            sprintf(titulo, "--- FASE %d ---", fase_atual + 1);
            centralizado(titulo);
            printf("\n\n");
            centralizado("LETRAS DISPONIVEIS:\n");
            printf("\n");

            char *letras_originais = lista_fases[fase_atual].letras_embaralhadas;
            char visual_formatado[200] = "";
            for (int k = 0; letras_originais[k] != '\0'; k++) {
                char temp[5];
                sprintf(temp, "%c", letras_originais[k]);
                strcat(visual_formatado, temp);
                if (letras_originais[k + 1] != '\0') {
                    strcat(visual_formatado, " -- ");
                }
            }
            centralizado(visual_formatado);
            printf("\n\n");

            for (int j = 0; j < lista_fases[fase_atual].total_respostas; j++) {
                imprimiresp((LARGURA_TELA - 20) / 2);
                if (lista_fases[fase_atual].acertos[j] == 1) {
                    printf("[ %s ]\n", lista_fases[fase_atual].respostas[j]);
                } else {
                    printf("[ ?????? ]\n");
                }
            }

            printf("\n");
            centralizado("Digite a palavra em MAIUSCULO (ou 'SAIR'): ");

            char linha_jogo[60];
            if (fgets(linha_jogo, sizeof(linha_jogo), stdin) == NULL) {
                // EOF: salva e sai
                salvar_progresso(nomes);
                return 0;
            }
            // Copia ate 49 chars removendo '\n'
            int kk = 0;
            while (linha_jogo[kk] != '\0' && linha_jogo[kk] != '\n' && kk < 49) {
                tentativa[kk] = linha_jogo[kk];
                kk++;
            }
            tentativa[kk] = '\0';

            if (tentativa[0] == '\0') continue; // linha vazia, pede de novo

            // Converte a tentativa para maiusculo automaticamente
            for (int k = 0; tentativa[k] != '\0'; k++) {
                tentativa[k] = toupper((unsigned char)tentativa[k]);
            }

            if (strcmp(tentativa, "SAIR") == 0) {
                salvar_progresso(nomes);
                return 0;
            }

            int acertou_agora = 0;
            for (int j = 0; j < lista_fases[fase_atual].total_respostas; j++) {
                if (strcmp(tentativa, lista_fases[fase_atual].respostas[j]) == 0 &&
                    lista_fases[fase_atual].acertos[j] == 0) {
                    lista_fases[fase_atual].acertos[j] = 1;
                    palavras_restantes--;
                    acertou_agora = 1;
                    break;
                }
            }

            if (acertou_agora) {
                printf("\n");
                printf("\033[1;32m");
                pontos += 100;
                centralizado("BOA! VOCE ACERTOU UMA PALAVRA!\n");
                centralizado("(Pressione Enter para continuar)");
                limpar_buffer();
            } else {
                printf("\n");
                printf("\033[1;31m");
                // Nao deixa a pontuacao ficar negativa
                if (pontos >= 10) {
                    pontos -= 10;
                } else {
                    pontos = 0;
                }
                centralizado("Palavra incorreta ou ja encontrada!\n");
                printf("\033[1;32m");
                centralizado("Tente de novo! (Enter para continuar)");
                limpar_buffer();
            }
        }

        limparTela();
        printf("\n\n\n");
        pontos += 25;
        centralizado("PARABENS! FASE CONCLUIDA!\n");
        printf("\n");
        centralizado("Pressione ENTER para a proxima fase...");
        limpar_buffer();
        fase_atual++;
    }

    limparTela();
    centralizado("VOCE ZEROU O JOGO! PARABENS!\n");
    salvar_progresso(nomes);

    pausar();

    return 0;
}

// Tela de introducao antes de comecar a jogar
int jogo() {
    char tamanhoooo[] = "01010101010101010101010101010101010101010101010101";
    int tu = contartama(tamanhoooo);
    int margem_menuuu = (LARGURA_TELA - tu) / 2;

    limparTela();
    printf("\033[1;32m");
    imprimiresp(margem_menuuu);
    printf("01010101010101010101010101010101010101010101010101\n");
    imprimiresp(margem_menuuu);
    printf("10101010101010101010101010101010101010101010101010\n");
    imprimiresp(margem_menuuu);
    printf("010                                            010\n");
    imprimiresp(margem_menuuu);
    printf("101      11111   00000   00111   00000         101\n");
    imprimiresp(margem_menuuu);
    printf("010        1    0     0  1       0     0       010\n");
    imprimiresp(margem_menuuu);
    printf("101        1    0     0  1 011   0     0       101\n");
    imprimiresp(margem_menuuu);
    printf("010     1  1    0     0  1    1  0     0       010\n");
    imprimiresp(margem_menuuu);
    printf("101      11      00000   00111   00000         101\n");
    imprimiresp(margem_menuuu);
    printf("010                                            010\n");
    imprimiresp(margem_menuuu);
    printf("10101010101010101010101010101010101010101010101010\n");
    imprimiresp(margem_menuuu);
    printf("010          V A I   C O M E C A R !           010\n");
    imprimiresp(margem_menuuu);
    printf("10101010101010101010101010101010101010101010101010\n");
    imprimiresp(margem_menuuu);
    printf("01010101010101010101010101010101010101010101010101\n");
    imprimiresp(margem_menuuu);

    pausar();

    return 1;
}

// Tela de confirmacao para apagar o ranking
void apagarranking() {
    int sel = 2;
    char linha[16];

    while (1) {
        limparTela();
        printf("\033[1;32m");
        printf("\n\n");
        centralizado("#####################################\n");
        centralizado("#       ZONA DE PERIGO: APAGAR      #\n");
        centralizado("#####################################\n");
        printf("\n");
        centralizado("VOCE ESTA PRESTES A EXCLUIR TODO\n");
        centralizado("O HISTORICO DE PONTUACAO DO JOGO.\n");
        printf("\n");
        centralizado("ESSA ACAO NAO PODE SER DESFEITA!\n");
        printf("\n");
        centralizado("-------------------------------------\n");
        printf("\n");

        if (sel == 1) {
            centralizado("   >>>  SIM, APAGAR TUDO  <<<    \n");
        } else {
            centralizado("        SIM, APAGAR TUDO         \n");
        }

        printf("\n");

        if (sel == 2) {
            centralizado("   >>>  NAO, VOLTAR AGORA <<<    \n");
        } else {
            centralizado("        NAO, VOLTAR AGORA        \n");
        }

        printf("\n");
        centralizado("-------------------------------------\n");
        centralizado("Use 'w' e 's' para escolher e 'f' para confirmar\n");
        printf("\n");
        centralizado("Sua decisao: ");

        if (fgets(linha, sizeof(linha), stdin) == NULL) break;
        char comando = linha[0];

        if (comando == 'w' || comando == 'W') {
            sel = 1;
        } else if (comando == 's' || comando == 'S') {
            sel = 2;
        } else if (comando == 'f' || comando == 'F') {
            if (sel == 1) {
                FILE *arq = fopen("ranking.bin", "wb");
                if (arq != NULL) {
                    fclose(arq);
                }
                limparTela();
                printf("\033[1;32m");
                printf("\n\n\n");
                centralizado("#####################################\n");
                centralizado("#     RANKING ZERADO COM SUCESSO    #\n");
                centralizado("#####################################\n");
                pausar();
                return;
            } else {
                limparTela();
                printf("\033[1;32m");
                printf("\n\n\n");
                centralizado("Ufa... Operacao cancelada!\n");
                centralizado("Seus dados estao seguros.\n");
                pausar();
                return;
            }
        }
    }
}

// Exibe e controla o menu principal
int menu() {
    int sel = 1;
    int tot = 5;
    char linha[16];
    char tamanhooo[] = "------------------------------";
    int tu = contartama(tamanhooo);
    int margem_menuu = (LARGURA_TELA - tu) / 2;

    while (1) {
        limparTela();
        printf("\033[1;32m");
        imprimiresp(margem_menuu);
        printf("------------------------------\n");
        if (sel == 1) {
            imprimiresp(margem_menuu);
            printf("   -> Configuracoes <-\n");
        } else {
            imprimiresp(margem_menuu);
            printf("      Configuracoes\n");
        }
        if (sel == 2) {
            imprimiresp(margem_menuu);
            printf("      -> Jogar  <-\n");
        } else {
            imprimiresp(margem_menuu);
            printf("         Jogar\n");
        }
        if (sel == 3) {
            imprimiresp(margem_menuu);
            printf("    -> Instrucoes <-\n");
        } else {
            imprimiresp(margem_menuu);
            printf("       Instrucoes\n");
        }
        if (sel == 4) {
            imprimiresp(margem_menuu);
            printf("     -> Ranking  <-\n");
        } else {
            imprimiresp(margem_menuu);
            printf("        Ranking\n");
        }
        if (sel == 5) {
            imprimiresp(margem_menuu);
            printf("   -> Sair do jogo <-\n");
        } else {
            imprimiresp(margem_menuu);
            printf("      Sair do jogo\n");
        }
        imprimiresp(margem_menuu);
        printf("------------------------------\n");
        centralizado("COMANDOS: 'w' (sobe), 's' (desce), 'f' (confirma/entra)\n");
        centralizado("Sua escolha: ");

        if (fgets(linha, sizeof(linha), stdin) == NULL) break;
        char comando = linha[0];

        if (comando == 'w' || comando == 'W') {
            sel--;
            if (sel < 1) sel = tot;
        } else if (comando == 's' || comando == 'S') {
            sel++;
            if (sel > tot) sel = 1;
        } else if (comando == 'f' || comando == 'F') {
            return sel;
        }
    }
    return 5; // EOF — retorna "Sair do jogo" por padrao
}

// Menu de configuracoes
int configuracao() {
    int sel = 1;
    int tot = 2;
    char linha[16];
    char tamanhoooo[] = "-------------------------------------";
    int tu = contartama(tamanhoooo);
    int margem_menuuu = (LARGURA_TELA - tu) / 2;

    while (1) {
        limparTela();
        printf("\033[1;32m");
        imprimiresp(margem_menuuu);
        printf("-------------------------------------\n");
        if (sel == 1) {
            imprimiresp(margem_menuuu);
            printf("      ->  zerar ranking  <-      \n");
        } else {
            imprimiresp(margem_menuuu);
            printf("          zerar ranking          \n");
        }
        if (sel == 2) {
            imprimiresp(margem_menuuu);
            printf("   -> voltar ao menu inicial <-  \n");
        } else {
            imprimiresp(margem_menuuu);
            printf("      voltar ao menu inicial     \n");
        }
        imprimiresp(margem_menuuu);
        printf("-------------------------------------\n");
        centralizado("COMANDOS: 'w' (sobe), 's' (desce), 'f' (confirma/entra)\n");
        centralizado("Sua escolha: ");

        if (fgets(linha, sizeof(linha), stdin) == NULL) break;

        // Verifica codigo secreto (easter egg): digitar "67" + ENTER
        if (linha[0] == '6' && linha[1] == '7') {
            pontos += 9999;
            limparTela();
            imprimiresp(margem_menuuu);
            printf("0101010101010101010101010101010101010\n");
            imprimiresp(margem_menuuu);
            printf("10101010101010 PARABENS! 010101010101\n");
            imprimiresp(margem_menuuu);
            printf("0101010101010101010101010101010101010\n");
            imprimiresp(margem_menuuu);
            printf("10101010101 VC DESBLOQUEOU 0101010101\n");
            imprimiresp(margem_menuuu);
            printf("0101010101010 UM PRESENTE 10101010101\n");
            imprimiresp(margem_menuuu);
            printf("1010101010101010101010101010101010101\n");
            pausar();
            continue;
        }

        char comando = linha[0];
        if (comando == 'w' || comando == 'W') {
            sel--;
            if (sel < 1) sel = tot;
        } else if (comando == 's' || comando == 'S') {
            sel++;
            if (sel > tot) sel = 1;
        } else if (comando == 'f' || comando == 'F') {
            return sel;
        }
    }
    return 2; // EOF — retorna "voltar ao menu inicial" por padrao
}

// Funcao principal que controla o fluxo do jogo
void chamafuncoesprincipais(char nomes[]) {
    int sair = 0;

    while (sair != 1) {
        int opcao = menu();

        if (opcao == 1) {
            if (configuracao() == 1) {
                apagarranking();
            }
        } else if (opcao == 2) {
            if (jogo() == 1) {
                jogar(nomes);
            }
        } else if (opcao == 3) {
            instrucoes();
        } else if (opcao == 4) {
            exibir_ranking();
        } else if (opcao == 5) {
            if (sairdojogo() == 1) {
                sair = 1;
            }
        }
    }
}

// Exibe mensagem de erro para nome invalido
void erro_palavra() {
    printf("\033[1;31m");
    printf("\n\n");
    centralizado("ERRO: Nome invalido! Use apenas letras (sem espacos ou numeros).\n");
    printf("\033[0m");
    pausar();
}

// Valida se o nome digitado contem apenas letras
void validar_nome(int* y, char nomes[], int* nome_valido) {
    *y = 0;
    *nome_valido = 1;

    if (nomes[0] == '\0') {
        *nome_valido = 0;
        return;
    }

    while (nomes[*y] != '\0') {
        if (!((nomes[*y] >= 'a' && nomes[*y] <= 'z') || (nomes[*y] >= 'A' && nomes[*y] <= 'Z'))) {
            *nome_valido = 0;
            break;
        }
        (*y)++;
    }
}

// Tela de boas-vindas e coleta do nome do jogador
void menu1(char* nomes) {
    char tamanhoo[] = "========================================";
    int tata = contartama(tamanhoo);
    int margem_menu = (LARGURA_TELA - tata) / 2;

    limparTela();
    printf("\033[1;32m");
    printf("\n");
    imprimiresp(margem_menu);
    printf("%s\n", tamanhoo);
    imprimiresp(margem_menu);
    printf("||                                    ||\n");
    imprimiresp(margem_menu);
    printf("||   BEM-VINDO AO MEU JOGO WMLETRAS!  ||\n");
    imprimiresp(margem_menu);
    printf("||      (O melhor jogo na real)       ||\n");
    imprimiresp(margem_menu);
    printf("||                                    ||\n");
    imprimiresp(margem_menu);
    printf("||    [ 0110 ]          [ 0111 ]      ||\n");
    imprimiresp(margem_menu);
    printf("%s\n", tamanhoo);
    printf("\n");
    centralizado("Antes de comecarmos...\n");
    centralizado("  Qual e o seu nome? (apenas letras, sem espacos): ");

    nomes[0] = '\0';
    char linha_nome[116];
    if (fgets(linha_nome, sizeof(linha_nome), stdin) != NULL) {
        int k = 0;
        while (linha_nome[k] != '\0' && linha_nome[k] != '\n' && k < 99) {
            nomes[k] = linha_nome[k];
            k++;
        }
        nomes[k] = '\0';
    }
}

// Funcao principal
int main() {
    char nomes[100];
    int y = 0, nome_valido = 0;

    do {
        menu1(nomes);
        validar_nome(&y, nomes, &nome_valido);
        if (nome_valido == 0) {
            erro_palavra();
        }
    } while (nome_valido == 0);

    chamafuncoesprincipais(nomes);

    terminarjogo();
    return 0;
}
