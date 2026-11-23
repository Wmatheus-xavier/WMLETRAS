/**
 * @file ui.c
 * @brief Implementação da interface de usuário do WMLETRAS com ncurses.
 *
 * Toda a saída é feita via ncurses (printw/mvprintw/attron).
 * A entrada é feita via getch() — sem pressionar ENTER para navegar.
 * ui_ler_linha() e ui_ler_linha_timeout() leem caractere a caractere
 * exibindo eco manual.
 */
#include "ui.h"

#include <ncurses.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

/* ── Ciclo de vida ──────────────────────────────────────────────────────── */

void ui_iniciar(void) {
    initscr();
    cbreak();           /* caractere imediato, sem buffer de linha       */
    noecho();           /* eco manual — controlamos o que aparece        */
    keypad(stdscr, TRUE); /* habilita teclas especiais (setas)           */
    curs_set(0);        /* cursor invisível por padrão                   */

    if (has_colors()) {
        start_color();
        use_default_colors();
        init_pair(PAR_VERDE,    COLOR_GREEN,  COLOR_BLACK);
        init_pair(PAR_VERMELHO, COLOR_RED,    COLOR_BLACK);
        init_pair(PAR_CIANO,    COLOR_CYAN,   COLOR_BLACK);
        init_pair(PAR_AMARELO,  COLOR_YELLOW, COLOR_BLACK);
        init_pair(PAR_NORMAL,   COLOR_WHITE,  COLOR_BLACK);
    }
}

void ui_encerrar(void) {
    endwin();
}

/* ── Terminal ───────────────────────────────────────────────────────────── */

int ui_largura_terminal(void) {
    return COLS > 0 ? COLS : LARGURA_PADRAO;
}

void ui_limpar_tela(void) {
    clear();
    refresh();
}

void ui_limpar_buffer(void) {
    flushinp();
}

void ui_centralizado(int largura, const char *texto) {
    int tam = (int)strlen(texto);
    int esp = (largura - tam) / 2;
    if (esp < 0) esp = 0;
    printw("%*s%s", esp, "", texto);
}

void ui_pausar(int largura) {
    printw("\n");
    ui_centralizado(largura, "Pressione qualquer tecla para continuar...");
    refresh();
    curs_set(0);
    nodelay(stdscr, FALSE); /* bloqueia ate tecla */
    getch();
}

/* ── Menu genérico ──────────────────────────────────────────────────────── */

#define MENU_CAIXA 37

static void menu_linha_nc(int margem, const char *conteudo) {
    printw("%*s%s\n", margem, "", conteudo);
}

int ui_menu(const char *titulo, const ItemMenu *itens, int n,
            int padrao_eof, int largura) {
    int sel  = 0;
    int marg = (largura - MENU_CAIXA) / 2;
    if (marg < 0) marg = 0;

    nodelay(stdscr, FALSE);
    curs_set(0);

    while (1) {
        clear();
        attron(COLOR_PAIR(PAR_VERDE) | A_BOLD);

        menu_linha_nc(marg, "+-----------------------------------+");
        if (titulo) {
            char cab[42];
            snprintf(cab, sizeof(cab), "| %-33s |", titulo);
            menu_linha_nc(marg, cab);
            menu_linha_nc(marg, "+-----------------------------------+");
        }

        for (int i = 0; i < n; i++) {
            char item[42];
            if (i == sel) {
                attron(COLOR_PAIR(PAR_AMARELO) | A_BOLD | A_REVERSE);
                snprintf(item, sizeof(item), "|  -> %-28s  |", itens[i].texto);
            } else {
                attron(COLOR_PAIR(PAR_VERDE) | A_BOLD);
                snprintf(item, sizeof(item), "|     %-28s  |", itens[i].texto);
            }
            menu_linha_nc(marg, item);
            attron(COLOR_PAIR(PAR_VERDE) | A_BOLD);
        }

        menu_linha_nc(marg, "+-----------------------------------+");

        attroff(A_BOLD | A_REVERSE);
        attron(COLOR_PAIR(PAR_NORMAL));
        ui_centralizado(largura, "setas/w-s=navegar  Enter/f=confirmar\n");
        attroff(COLOR_PAIR(PAR_NORMAL));

        refresh();

        int ch = getch();
        if (ch == ERR) return padrao_eof;

        if (ch == KEY_UP   || ch == 'w' || ch == 'W') {
            sel = (sel - 1 + n) % n;
        } else if (ch == KEY_DOWN || ch == 's' || ch == 'S') {
            sel = (sel + 1) % n;
        } else if (ch == '\n' || ch == '\r' || ch == 'f' || ch == 'F') {
            return sel;
        }
    }
}

/* ── Entrada de texto ───────────────────────────────────────────────────── */

/*
 * Lê caractere a caractere com eco manual.
 * Retorna 1 (OK), 0 (ERR/EOF), 2 (timeout — só via ui_ler_linha_timeout).
 */
int ui_ler_linha(char *buf, int max_chars) {
    return ui_ler_linha_timeout(buf, max_chars, 0, NULL, NULL);
}

int ui_ler_linha_timeout(char *buf, int max_chars, int timeout_s,
                         void (*on_tick)(int segundos_restantes, void *dados),
                         void *dados_tick) {
    buf[0] = '\0';
    int len = 0;

    curs_set(1); /* mostra cursor durante digitação */

    /* Captura posição inicial do cursor para o eco */
    int row, col;
    getyx(stdscr, row, col);

    time_t inicio = time(NULL);
    time_t ultimo_tick = inicio;

    /* Configura timeout ncurses: 100 ms por getch para podermos atualizar */
    if (timeout_s > 0)
        timeout(100);   /* 100 ms */
    else
        nodelay(stdscr, FALSE);

    int resultado = 1;

    while (1) {
        /* Atualiza cronômetro */
        if (timeout_s > 0) {
            time_t agora = time(NULL);
            int decorrido = (int)(agora - inicio);
            int restantes  = timeout_s - decorrido;

            if (restantes <= 0) {
                resultado = 2; /* timeout */
                break;
            }

            /* Dispara tick a cada segundo */
            if (agora != ultimo_tick) {
                ultimo_tick = agora;
                if (on_tick) on_tick(restantes, dados_tick);
                refresh();
            }
        }

        int ch = getch();
        if (ch == ERR) {
            /* Com timeout ncurses, ERR significa apenas que não houve tecla */
            if (timeout_s > 0) continue;
            resultado = 0;
            break;
        }

        if (ch == '\n' || ch == '\r') {
            break;  /* confirmação */
        }

        if (ch == KEY_BACKSPACE || ch == 127 || ch == '\b') {
            if (len > 0) {
                len--;
                buf[len] = '\0';
                /* Apaga o último caractere na tela */
                int r2, c2;
                getyx(stdscr, r2, c2);
                (void)r2;
                if (c2 > col || (c2 == 0 && row > 0)) {
                    mvprintw(row, col, "%-*s", max_chars, buf);
                    int nc = col + len;
                    move(row, nc < COLS ? nc : COLS - 1);
                    refresh();
                }
            }
            continue;
        }

        /* Aceita apenas caracteres imprimíveis ASCII */
        if (isprint(ch) && len < max_chars) {
            buf[len++] = (char)ch;
            buf[len]   = '\0';
            /* Reimprime para manter alinhamento */
            mvprintw(row, col, "%-*s", max_chars + 1, buf);
            move(row, col + len);
            refresh();
        }
    }

    /* Restaura timeout e cursor */
    if (timeout_s > 0) timeout(-1);
    nodelay(stdscr, FALSE);
    curs_set(0);

    return resultado;
}

/* ── Telas fixas ────────────────────────────────────────────────────────── */

void ui_tela_boas_vindas(EstadoJogo *estado) {
    int largura = estado->largura_tela;
    int marg    = (largura - 40) / 2;
    if (marg < 0) marg = 0;

    int nome_ok = 0;
    while (!nome_ok) {
        clear();
        attron(COLOR_PAIR(PAR_VERDE) | A_BOLD);
        printw("\n");
        printw("%*s%s\n", marg, "", "========================================");
        printw("%*s%s\n", marg, "", "||                                    ||");
        printw("%*s%s\n", marg, "", "||   BEM-VINDO AO MEU JOGO WMLETRAS!  ||");
        printw("%*s%s\n", marg, "", "||        ( estilo Matrix )           ||");
        printw("%*s%s\n", marg, "", "||                                    ||");
        printw("%*s%s\n", marg, "", "||    [ 0110 ]          [ 0111 ]      ||");
        printw("%*s%s\n", marg, "", "========================================");
        printw("\n");
        attroff(A_BOLD);

        attron(COLOR_PAIR(PAR_NORMAL));
        ui_centralizado(largura, "Qual e o seu nome? (apenas letras, sem espacos): ");
        attroff(COLOR_PAIR(PAR_NORMAL));
        refresh();

        char buf[MAX_NOME];
        buf[0] = '\0';
        if (!ui_ler_linha(buf, MAX_NOME - 1)) {
            strncpy(estado->nome_jogador, "Jogador", MAX_NOME - 1);
            estado->nome_jogador[MAX_NOME - 1] = '\0';
            return;
        }

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
            printw("\n");
            attron(COLOR_PAIR(PAR_VERMELHO) | A_BOLD);
            ui_centralizado(largura,
                "Nome invalido! Use apenas letras (sem espacos ou numeros).\n");
            attroff(COLOR_PAIR(PAR_VERMELHO) | A_BOLD);
            ui_pausar(largura);
        }
    }
}

void ui_instrucoes(int largura) {
    int marg = (largura - 37) / 2;
    if (marg < 0) marg = 0;

    clear();
    attron(COLOR_PAIR(PAR_VERDE) | A_BOLD);
    printw("%*s%s\n", marg, "", "+-----------------------------------+");
    printw("%*s%s\n", marg, "", "|        COMO JOGAR WMLETRAS        |");
    printw("%*s%s\n", marg, "", "+-----------------------------------+");
    printw("%*s%s\n", marg, "", "|                                   |");
    printw("%*s%s\n", marg, "", "| 1. Veja as letras disponiveis.    |");
    printw("%*s%s\n", marg, "", "| 2. Forme palavras com elas.       |");
    printw("%*s%s\n", marg, "", "| 3. Acerte todas para passar fase. |");
    printw("%*s%s\n", marg, "", "|                                   |");
    printw("%*s%s\n", marg, "", "| Acertou:    +100 pts              |");
    printw("%*s%s\n", marg, "", "| Errou:       -10 pts (minimo 0)   |");
    printw("%*s%s\n", marg, "", "| Passou fase:  +25 pts             |");
    printw("%*s%s\n", marg, "", "|                                   |");
    printw("%*s%s\n", marg, "", "| MODO DESAFIO:                     |");
    printw("%*s%s\n", marg, "", "| Cada fase tem cronometro. Acertos |");
    printw("%*s%s\n", marg, "", "| rapidos dao bonus de ate +50 pts. |");
    printw("%*s%s\n", marg, "", "| Tempo esgotado = fase encerrada.  |");
    printw("%*s%s\n", marg, "", "|                                   |");
    printw("%*s%s\n", marg, "", "| DICIONARIO:                       |");
    printw("%*s%s\n", marg, "", "| Tentativas invalidas (nao sao     |");
    printw("%*s%s\n", marg, "", "| palavras reais) sao rejeitadas    |");
    printw("%*s%s\n", marg, "", "| sem perder pontos.                |");
    printw("%*s%s\n", marg, "", "|                                   |");
    printw("%*s%s\n", marg, "", "| Digite SAIR para salvar e sair.   |");
    printw("%*s%s\n", marg, "", "|                                   |");
    printw("%*s%s\n", marg, "", "+-----------------------------------+");
    attroff(COLOR_PAIR(PAR_VERDE) | A_BOLD);
    refresh();

    ui_pausar(largura);
}

void ui_tela_fim(int largura) {
    clear();
    printw("\n\n\n\n");
    attron(COLOR_PAIR(PAR_VERDE) | A_BOLD);
    ui_centralizado(largura, "Obrigado por jogar WMLETRAS!\n");
    ui_centralizado(largura, "Ate a proxima, jogador!\n");
    attroff(COLOR_PAIR(PAR_VERDE) | A_BOLD);
    printw("\n\n\n\n");
    refresh();
    ui_pausar(largura);
}

void ui_tela_inicio_jogo(int largura) {
    int marg = (largura - 50) / 2;
    if (marg < 0) marg = 0;

    clear();
    attron(COLOR_PAIR(PAR_VERDE) | A_BOLD);
    printw("%*s%s\n", marg, "", "01010101010101010101010101010101010101010101010101");
    printw("%*s%s\n", marg, "", "10101010101010101010101010101010101010101010101010");
    printw("%*s%s\n", marg, "", "010                                            010");
    printw("%*s%s\n", marg, "", "101      11111   00000   00111   00000         101");
    printw("%*s%s\n", marg, "", "010        1    0     0  1       0     0       010");
    printw("%*s%s\n", marg, "", "101        1    0     0  1 011   0     0       101");
    printw("%*s%s\n", marg, "", "010     1  1    0     0  1    1  0     0       010");
    printw("%*s%s\n", marg, "", "101      11      00000   00111   00000         101");
    printw("%*s%s\n", marg, "", "010                                            010");
    printw("%*s%s\n", marg, "", "10101010101010101010101010101010101010101010101010");
    printw("%*s%s\n", marg, "", "010          V A I   C O M E C A R !           010");
    printw("%*s%s\n", marg, "", "10101010101010101010101010101010101010101010101010");
    printw("%*s%s\n", marg, "", "01010101010101010101010101010101010101010101010101");
    attroff(COLOR_PAIR(PAR_VERDE) | A_BOLD);
    refresh();

    ui_pausar(largura);
}
