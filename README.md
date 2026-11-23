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

| Tecla | Ação                    |
|-------|-------------------------|
| `W`   | Mover seleção para cima |
| `S`   | Mover seleção para baixo|
| `F`   | Confirmar / Entrar      |

### Pontuação

| Evento             | Pontos         |
|--------------------|----------------|
| Palavra correta    | +100           |
| Palavra errada     | -10 (mín. 0)   |
| Passar de fase     | +25            |

---

## Instalação e compilação

### Pré-requisitos
- GCC ≥ 9 (ou clang ≥ 11)
- make
- Linux, macOS ou Windows com MinGW/MSYS2

### Compilar e executar

```bash
git clone https://github.com/seu-usuario/wmletras.git
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
│   ├── tipos.h           # Structs, constantes e cores ANSI compartilhadas
│   ├── ui.h              # Interface de usuário (menus, tela, entrada)
│   ├── fases.h           # Carregamento e validação de fases
│   ├── ranking.h         # Leitura/escrita do ranking
│   └── jogo.h            # Lógica da partida
├── src/
│   ├── main.c            # Ponto de entrada e fluxo principal
│   ├── ui.c              # Implementação da UI
│   ├── fases.c           # Implementação do carregador de fases
│   ├── ranking.c         # Implementação do ranking
│   └── jogo.c            # Implementação da partida
├── tests/
│   ├── test_framework.h  # Micro-framework de testes (sem dependências)
│   ├── test_fases.c      # 12 testes para o módulo de fases
│   └── test_ranking.c    # 4 testes para o módulo de ranking
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
| `ui_menu()` genérico | Um único lugar para mudar o estilo do cursor `->` |
| ANSI `\033[2J\033[H` em vez de `system("clear")` | Mais portátil e seguro (sem risco de shell injection) |
| `fscanf` + validação `resposta_valida()` | Detecta erros no `entrada.txt` em tempo de carga |
| Testes sem framework externo | Sem dependências; `make test` funciona em qualquer máquina com gcc |

---

## Referências de aprendizado

- Abertura de arquivos em C: <https://youtu.be/MtYuhRtQPHU>
- Limpeza de tela via ANSI: <https://stackoverflow.com/q/55672661>
- Cores no terminal: <https://youtu.be/Hl-7xMl7bn0>

---

## Ideias de evolução futura

1. **Modo desafio com tempo** — cada fase tem um cronômetro regressivo; acertos rápidos dão bônus extra.
2. **Dicionário embutido** — validar que a tentativa é uma palavra real do português antes de comparar com as respostas, eliminando tentativas "AAAA".
3. **Interface ncurses** — substituir `printf` por ncurses para posicionamento preciso de cursor, animações e entrada em tempo real (sem precisar apertar ENTER).
