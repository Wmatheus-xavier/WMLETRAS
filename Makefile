# =============================================================================
# Makefile — WMLETRAS
# Uso: make [all | run | debug | test | asan | clean]
# =============================================================================

CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -Wpedantic -Werror -Iinclude
LDFLAGS :=

# Flags extras para o target debug (símbolos + sem otimização)
DBGFLAGS := -g -O0 -DDEBUG

# Flags do AddressSanitizer / UndefinedBehaviorSanitizer
SANFLAGS := -fsanitize=address,undefined -g -O1

SRC_DIR  := src
TEST_DIR := tests
OBJ_DIR  := build/obj

# Fontes do jogo (todos os .c em src/ exceto main.c — para poder linkar nos testes)
LIB_SRCS := $(SRC_DIR)/ui.c \
            $(SRC_DIR)/fases.c \
            $(SRC_DIR)/ranking.c \
            $(SRC_DIR)/jogo.c

MAIN_SRC := $(SRC_DIR)/main.c

# Objetos derivados
LIB_OBJS  := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(LIB_SRCS))
MAIN_OBJ  := $(OBJ_DIR)/main.o

TARGET    := wmletras

# =============================================================================
# Targets principais
# =============================================================================

.PHONY: all run debug test asan clean

all: $(TARGET)

$(TARGET): $(LIB_OBJS) $(MAIN_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "[ok] $(TARGET) compilado com sucesso."

# Compila cada .c em build/obj/
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Executa o jogo (certifique-se de rodar de dentro da raiz do projeto)
run: all
	./$(TARGET)

# Build com símbolos de depuração
debug: $(LIB_SRCS) $(MAIN_SRC)
	$(CC) $(CFLAGS) $(DBGFLAGS) $^ -o $(TARGET)_debug $(LDFLAGS)
	@echo "[ok] $(TARGET)_debug compilado."

# Build + execução com AddressSanitizer e UBSanitizer
asan: $(LIB_SRCS) $(MAIN_SRC)
	$(CC) $(CFLAGS) $(SANFLAGS) $^ -o $(TARGET)_asan $(LDFLAGS)
	@echo "[ok] $(TARGET)_asan compilado. Execute: ./$(TARGET)_asan"

# =============================================================================
# Testes
# =============================================================================

# Cada arquivo tests/test_*.c tem seu próprio main(); compilamos e rodamos
# cada um separadamente para isolamento de falhas.
TEST_SRCS := $(wildcard $(TEST_DIR)/test_*.c)
TEST_BINS := $(patsubst $(TEST_DIR)/%.c, build/%, $(TEST_SRCS))

build/%: $(TEST_DIR)/%.c $(LIB_OBJS) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(TEST_DIR) $< $(LIB_OBJS) -o $@ $(LDFLAGS)

test: $(TEST_BINS)
	@echo ""
	@FAIL=0; \
	for t in $(TEST_BINS); do \
	    echo "--- $$t ---"; \
	    ./$$t || FAIL=1; \
	done; \
	if [ $$FAIL -eq 0 ]; then \
	    echo "[PASS] Todos os testes passaram."; \
	else \
	    echo "[FAIL] Algum teste falhou."; exit 1; \
	fi

# =============================================================================
# Limpeza
# =============================================================================

clean:
	rm -rf build/ $(TARGET) $(TARGET)_debug $(TARGET)_asan
	@echo "[ok] Limpeza concluida."
