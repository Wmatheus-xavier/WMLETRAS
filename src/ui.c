/**
 * @file ui.c
 * @brief Implementação da interface de usuário do WMLETRAS.
 */
#include "ui.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* ── Detecção de largura do terminal ─────────────────────────────────────── */
#ifdef _WIN32
#  include <windows.h>
int ui_largura_terminal(void) {
    CONSOLE_SCREEN_BUFFER_INFO info;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info))
        return (int)(info.srWindow.Right - info.srWindow.Left + 1);
    return LARGURA_PADRAO;
}
#else
#  include <sys/ioctl.h>
#  include <unistd.h>
int ui_largura_terminal(void) {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_col > 0)
        return (int)w.ws_col;
    return LARGURA_PADRAO;
}
#endif

/* ── Utilitários de terminal ─────────────────────────────────────────────── */

void ui_limpar_tela(void) {
    /* ESC[2J: apaga a tela; ESC[H: cursor para canto superior esquerdo.
       Funciona em qualquer terminal ANSI/VT100 sem precisar de system(). */
    fputs("\033[2J\033[H", stdout);
    fflush(stdout);
}

void ui_limpar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

void ui_centralizado(int largura, const char *texto) {
    int tam = (int)strlen(texto);
    int esp = (largura - tam) / 2;
    if (esp < 0) esp = 0;
    printf("%*s%s", esp, "", texto);
}

void ui_pausar(int largura) {
    printf("\n");
    ui_centralizado(largura, "Pressione ENTER para continuar...");
    ui_limpar_buffer();
}

/* ── Menu genérico ──────────────────────────────────────────────────────── */

/*
 * Largura fixa da caixa do menu (em caracteres).
 * Usada internamente para calcular a margem de centralização.
 */
#define MENU_LARGURA_CAIXA 37

static void menu_linha(int margem, const char *conteudo) {
    printf("%*s%s\n", margem, "", conteudo);
}

int ui_menu(const char *titulo, const ItemMenu *itens, int n,
            int padrao_eof, int largura) {
    int sel  = 0;
    int marg = (largura - MENU_LARGURA_CAIXA) / 2;
    if (marg < 0) marg = 0;

    char linha_in[16];

    while (1) {
        ui_limpar_tela();
        printf("%s", COR_VERDE);

        /* Cabeçalho */
        menu_linha(marg, "+-----------------------------------+");
        if (titulo) {
            char cabecalho[40];
            snprintf(cabecalho, sizeof(cabecalho), "| %-33s |", titulo);
            menu_linha(marg, cabecalho);
            menu_linha(marg, "+-----------------------------------+");
        }

        /* Itens */
        for (int i = 0; i < n; i++) {
            char item_fmt[40];
            if (i == sel)
                snprintf(item_fmt, sizeof(item_fmt), "|  -> %-28s  |", itens[i].texto);
            else
                snprintf(item_fmt, sizeof(item_fmt), "|     %-28s  |", itens[i].texto);
            menu_linha(marg, item_fmt);
        }
        menu_linha(marg, "+-----------------------------------+");

        /* Legenda */
        ui_centralizado(largura, "w=sobe  s=desce  f=confirma\n");
        ui_centralizado(largura, "> ");

        /* Leitura */
        if (fgets(linha_in, sizeof(linha_in), stdin) == NULL)
            return padrao_eof;

        char cmd = (char)tolower((unsigned char)linha_in[0]);
        if (cmd == 'w') {
            sel = (sel - 1 + n) % n;
        } else if (cmd == 's') {
            sel = (sel + 1) % n;
        } else if (cmd == 'f') {
            return sel;
        }
    }
}

/* ── Entrada de texto ───────────────────────────────────────────────────── */

int ui_ler_linha(char *buf, int max_chars) {
    char tmp[MAX_NOME + 4];
    if (fgets(tmp, (int)sizeof(tmp), stdin) == NULL)
        return 0;
    int k = 0;
    while (tmp[k] != '\0' && tmp[k] != '\n' && k < max_chars) {
        buf[k] = tmp[k];
        k++;
    }
    buf[k] = '\0';
    return 1;
}

/* ── Telas fixas ────────────────────────────────────────────────────────── */

void ui_tela_boas_vindas(EstadoJogo *estado) {
    int    largura = estado->largura_tela;
    int    marg    = (largura - 40) / 2;
    if (marg < 0) marg = 0;

    int nome_ok = 0;
    while (!nome_ok) {
        ui_limpar_tela();
        printf("%s", COR_VERDE);
        printf("\n");
        printf("%*s%s\n", marg, "", "========================================");
        printf("%*s%s\n", marg, "", "||                                    ||");
        printf("%*s%s\n", marg, "", "||   BEM-VINDO AO MEU JOGO WMLETRAS!  ||");
        printf("%*s%s\n", marg, "", "||        ( estilo Matrix )           ||");
        printf("%*s%s\n", marg, "", "||                                    ||");
        printf("%*s%s\n", marg, "", "||    [ 0110 ]          [ 0111 ]      ||");
        printf("%*s%s\n", marg, "", "========================================");
        printf("\n");
        ui_centralizado(largura, "Qual e o seu nome? (apenas letras, sem espacos): ");

        char buf[MAX_NOME];
        if (!ui_ler_linha(buf, MAX_NOME - 1)) {
            /* EOF — usa nome padrão para não travar */
            strncpy(estado->nome_jogador, "Jogador", MAX_NOME - 1);
            estado->nome_jogador[MAX_NOME - 1] = '\0';
            return;
        }

        /* Valida: só letras, pelo menos 1 char */
        int valido = (buf[0] != '\0');
        for (int i = 0; buf[i] != '\0' && valido; i++) {
            if (!isalpha((unsigned char)buf[i]))
                valido = 0;
        }

        if (valido) {
            strncpy(estado->nome_jogador, buf, MAX_NOME - 1);
            estado->nome_jogador[MAX_NOME - 1] = '\0';
            nome_ok = 1;
        } else {
            printf("\n%s", COR_VERMELHO);
            ui_centralizado(largura,
                "Nome invalido! Use apenas letras (sem espacos ou numeros).\n");
            printf("%s", COR_RESET);
            ui_pausar(largura);
        }
    }
}

void ui_instrucoes(int largura) {
    int marg = (largura - 37) / 2;
    if (marg < 0) marg = 0;

    ui_limpar_tela();
    printf("%s", COR_VERDE);
    printf("%*s%s\n", marg, "", "+-----------------------------------+");
    printf("%*s%s\n", marg, "", "|        COMO JOGAR WMLETRAS        |");
    printf("%*s%s\n", marg, "", "+-----------------------------------+");
    printf("%*s%s\n", marg, "", "|                                   |");
    printf("%*s%s\n", marg, "", "| 1. Veja as letras disponiveis.    |");
    printf("%*s%s\n", marg, "", "| 2. Forme palavras com elas.       |");
    printf("%*s%s\n", marg, "", "| 3. Acerte todas para passar fase. |");
    printf("%*s%s\n", marg, "", "|                                   |");
    printf("%*s%s\n", marg, "", "| Acertou:   +100 pts               |");
    printf("%*s%s\n", marg, "", "| Errou:      -10 pts (minimo 0)    |");
    printf("%*s%s\n", marg, "", "| Passou fase: +25 pts              |");
    printf("%*s%s\n", marg, "", "|                                   |");
    printf("%*s%s\n", marg, "", "| Digite SAIR para salvar e sair.   |");
    printf("%*s%s\n", marg, "", "|                                   |");
    printf("%*s%s\n", marg, "", "+-----------------------------------+");
    printf("%s", COR_RESET);

    ui_pausar(largura);
}

void ui_tela_fim(int largura) {
    ui_limpar_tela();
    printf("%s\n\n\n\n", COR_VERDE);
    ui_centralizado(largura, "Obrigado por jogar WMLETRAS!\n");
    ui_centralizado(largura, "Ate a proxima, jogador!\n");
    printf("\n\n\n\n%s", COR_RESET);
}

void ui_tela_inicio_jogo(int largura) {
    int marg = (largura - 50) / 2;
    if (marg < 0) marg = 0;

    ui_limpar_tela();
    printf("%s", COR_VERDE);
    printf("%*s%s\n", marg, "", "01010101010101010101010101010101010101010101010101");
    printf("%*s%s\n", marg, "", "10101010101010101010101010101010101010101010101010");
    printf("%*s%s\n", marg, "", "010                                            010");
    printf("%*s%s\n", marg, "", "101      11111   00000   00111   00000         101");
    printf("%*s%s\n", marg, "", "010        1    0     0  1       0     0       010");
    printf("%*s%s\n", marg, "", "101        1    0     0  1 011   0     0       101");
    printf("%*s%s\n", marg, "", "010     1  1    0     0  1    1  0     0       010");
    printf("%*s%s\n", marg, "", "101      11      00000   00111   00000         101");
    printf("%*s%s\n", marg, "", "010                                            010");
    printf("%*s%s\n", marg, "", "10101010101010101010101010101010101010101010101010");
    printf("%*s%s\n", marg, "", "010          V A I   C O M E C A R !           010");
    printf("%*s%s\n", marg, "", "10101010101010101010101010101010101010101010101010");
    printf("%*s%s\n", marg, "", "01010101010101010101010101010101010101010101010101");

    ui_pausar(largura);
}
