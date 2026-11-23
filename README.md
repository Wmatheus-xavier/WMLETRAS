# WMLETRAS 🟩

> Jogo de anagramas para o terminal, escrito em C11, estilo Matrix/hacker.

**Autor:** Washington Matheus Almeida Xavier — matrícula 252024622  
**Disciplina:** Algoritmos e Programação de Computadores (APC)  
**Licença:** [MIT](LICENSE)

---

## Como jogar

Cada fase apresenta um conjunto de letras embaralhadas. Forme palavras usando **apenas** essas letras e as encontre todas para avançar.

```
+-----------------------------------+
|  Fase 2/15  |  Palavras: 3/6  |  Pontos: 325  |  Jogador: Theus
|
|   LETRAS DISPONIVEIS:
|
|   I -- A -- L -- M -- T
|
|   [ LATIM      ]
|   [ ??????     ]
|   [ ??????     ]
|   [ MIL        ]
|   [ ??????     ]
|   [ ??????     ]
|
|   Digite a palavra (ou SAIR para salvar e sair): _
```

### Controles

| Tecla / Tecla Especial | Ação                      |
|------------------------|---------------------------|
| `↑` / `W`              | Mover seleção para cima   |
| `↓` / `S`              | Mover seleção para baixo  |
| `Enter` / `F`          | Confirmar / Entrar        |

> **Entrada sem ENTER:** graças ao ncurses, todas as teclas dos menus são capturadas imediatamente, sem precisar pressionar ENTER.

### Pontuação

| Evento                              | Pontos            |
|-------------------------------------|-------------------|
| Palavra correta                     | +100              |
| Palavra errada                      | −10 (mín. 0)      |
| Passar de fase                      | +25               |
| **Acerto rápido (modo desafio)**    | **+0 a +50 bônus** |

### Modo Desafio 🕐

- Cada fase tem um **cronômetro regressivo** configurável (30 / 45 / 60 / 90 s).
- O HUD mostra o tempo restante **ao vivo**, colorindo verde → amarelo → vermelho.
- Acertos rápidos ganham bônus proporcional ao tempo restante (máx. +50 pts).
- Tempo esgotado: a fase encerra automaticamente sem bônus de conclusão.
- Configure em **"Configurar desafio"** no menu principal.

### Dicionário embutido 📖

- As tentativas são primeiro comparadas com as respostas válidas da fase. **Se a tentativa acerta, ela é aceita imediatamente**, independentemente do dicionário.
- Apenas tentativas que **não são respostas da fase** são verificadas no dicionário de **1 536 palavras** do português (sem acento). Palavras desconhecidas (ex.: "AAAA", "ZZZX") são rejeitadas **sem perda de pontos**.
- O dicionário pode ser desligado na tela **"Configurar desafio"**.

---

## Instalação e compilação

### Pré-requisitos
- GCC ≥ 9 (ou clang ≥ 11)
- make
- **libncurses-dev** (ou equivalente)
- Linux, macOS ou Windows com MinGW/MSYS2 + PDCurses

### Instalar ncurses (Debian/Ubuntu)

```bash
sudo apt-get install libncurses-dev
```

Caso não tenha sudo, o Makefile detecta automaticamente a ausência do header
e usa a cópia local em `deps/` (criada pelo script abaixo):

```bash
apt-get download libncurses-dev
dpkg-deb -x libncurses-dev_*.deb deps/
```

### Compilar e executar

```bash
git clone https://github.com/Wmatheus-xavier/Sistema-Jogo-C.git
cd wmletras
make        # compila
make run    # compila e executa
```

> **Importante:** execute sempre a partir da raiz do projeto (onde está o `Makefile`), pois o jogo procura `data/entrada.txt` como caminho relativo.

### Outros targets do Makefile

| Comando        | O que faz                                      |
|----------------|------------------------------------------------|
| `make all`     | Compila com `-Wall -Wextra -Wpedantic -Werror` |
| `make run`     | Compila e executa                              |
| `make debug`   | Compila com `-g -O0` para usar com gdb         |
| `make asan`    | Compila com AddressSanitizer + UBSanitizer     |
| `make test`    | Roda os testes unitários em `tests/`           |
| `make clean`   | Remove binários e artefatos de build           |

---

## Estrutura do projeto

```
wmletras/
├── data/
│   └── entrada.txt       # Fases do jogo (editável)
├── include/
│   ├── tipos.h           # Structs, constantes compartilhadas
│   ├── ui.h              # UI ncurses (menus, cronômetro, entrada sem ENTER)
│   ├── fases.h           # Carregamento e validação de fases
│   ├── ranking.h         # Leitura/escrita do ranking
│   ├── jogo.h            # Lógica da partida
│   └── dict.h            # Dicionário embutido de palavras PT-BR
├── src/
│   ├── main.c            # Ponto de entrada, modo desafio, fluxo principal
│   ├── ui.c              # Implementação ncurses da UI
│   ├── fases.c           # Implementação do carregador de fases
│   ├── ranking.c         # Implementação do ranking
│   ├── jogo.c            # Lógica da partida + cronômetro + dicionário
│   └── dict.c            # 1536 palavras PT-BR em tabela hash (DICT_SIZE=3307)
├── tests/
│   ├── test_framework.h  # Micro-framework de testes (sem dependências)
│   ├── test_dict.c       # 24 testes para o dicionário e regressão das fases
│   ├── test_fases.c      # 12 testes para o módulo de fases
│   └── test_ranking.c    #  4 testes para o módulo de ranking
├── Makefile
├── LICENSE
└── README.md
```

---

## Formato do `data/entrada.txt`

```
# Comentários começam com #
LETRAS_DA_FASE
NUMERO_DE_RESPOSTAS
RESPOSTA1
RESPOSTA2
...
-
```

**Regras:**
- `LETRAS_DA_FASE`: string sem espaços, maiúsculas ou minúsculas (convertidas automaticamente).
- `NUMERO_DE_RESPOSTAS`: inteiro de 1 a 20.
- Cada `RESPOSTA` deve ser formável com as letras da fase — o jogo valida isso na carga e ignora respostas inválidas com aviso no stderr.
- O separador `-` marca o fim de cada fase.
- Suporta até 50 fases.
- Linhas iniciadas com `#` são ignoradas (comentários).

**Exemplo:**
```
# Fase fácil — letras de RAAUG
RAAUG
5
RUGA
GRAU
RUA
AGUA
AGUAR
-
```

---

## Easter egg 🥚

Há um código secreto escondido na tela de **Configurações**. Os pontos bônus são acumulados e aplicados na sua próxima partida. Tente descobrir!

---

## Decisões técnicas

| Decisão | Justificativa |
|---------|---------------|
| Arquivos separados por módulo | Facilita leitura, teste e manutenção independente |
| `EstadoJogo` passado por ponteiro | Elimina variáveis globais; estado localizável e testável |
| Formato de ranking campo a campo | Evita dependência de padding/endianness do compilador |
| `ui_menu()` genérico com ncurses | Entrada em tempo real sem ENTER; setas funcionam |
| Cronômetro via `time()` + `timeout(100)` ncurses | Sem threads; callback de tick a cada segundo no getch loop |
| Dicionário como tabela hash estática | Zero alocação dinâmica; `dict_existe()` em O(1) médio |
| `ui_ler_linha_timeout()` com callback | Separa I/O de lógica; cronômetro atualizável sem redesenho total |
| `fscanf` + validação `resposta_valida()` | Detecta erros no `entrada.txt` em tempo de carga |
| Testes sem framework externo | Sem dependências; `make test` funciona em qualquer máquina com gcc |

---

## Referências de aprendizado

- Abertura de arquivos em C: <https://youtu.be/MtYuhRtQPHU>
- Limpeza de tela via ANSI: <https://stackoverflow.com/q/55672661>
- Cores no terminal: <https://youtu.be/Hl-7xMl7bn0>

---

## Implementado ✅

As ideias abaixo estavam listadas como "evolução futura" no commit inicial e
foram totalmente implementadas no código local (branch `main`):

| # | Ideia original | Onde encontrar |
|---|----------------|----------------|
| 1 | **Modo desafio com tempo** — cronômetro regressivo por fase; acertos rápidos dão bônus proporcional ao tempo restante (máx. +50 pts, `BONUS_RAPIDO_MAX`). | [`src/jogo.c`](src/jogo.c) · [`src/main.c`](src/main.c) · [`include/tipos.h`](include/tipos.h) |
| 2 | **Dicionário embutido** — 1 536 palavras PT-BR em tabela hash; tentativas inválidas rejeitadas sem penalidade; desligável no menu. | [`src/dict.c`](src/dict.c) · [`include/dict.h`](include/dict.h) |
| 3 | **Interface ncurses** — `printf` substituído por `printw`/`mvprintw`; menus com setas sem ENTER; entrada em tempo real com `getch()`; HUD do cronômetro atualizado ao vivo via `mvprintw`. | [`src/ui.c`](src/ui.c) · [`include/ui.h`](include/ui.h) |

---

## Ideias de evolução futura 🔭

1. **Mais fases** — ampliar `data/entrada.txt` com novas letras e respostas (contribuições bem-vindas!).
2. **Feedback visual de erro** — chamar `beep()` / `flash()` do ncurses ao errar uma palavra, em vez de apenas texto vermelho.
3. **Animação de letra revelada** — usar `mvchgat()` para piscar o slot de resposta quando uma palavra é acertada, sem redesenhar toda a tela.
4. **Suporte a acentos** — migrar de `ncurses` + `char` para `ncursesw` + `wchar_t` para aceitar ã/õ/é/etc. tanto na entrada quanto nas respostas (branch recomendado: `feat/acentos`).
