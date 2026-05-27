SRC_DIR = ./src
BIN_DIR = ./bin
TEST_DIR = ./src/test

# Executável final
TARGET = tradutor

# Arquivos fonte
SCANNER = $(SRC_DIR)/$(TARGET).l
PARSER = $(SRC_DIR)/$(TARGET).y

# parametros das ferramentas
CC = gcc
CFLAGS = -O2 -Wall -g -Iinclude -DDEBUG
BISON = bison
FLEX = flex

# Regra principal
all: compilar
	./$(BIN_DIR)/$(TARGET)

compilar: clean parser scanner
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/$(TARGET) $(BIN_DIR)/$(TARGET).tab.c $(BIN_DIR)/lex.yy.c $(SRC_DIR)/symtab.c $(SRC_DIR)/ast.c $(SRC_DIR)/mathfuncs.c -lm 

testar: clean
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -DTEST -o $(BIN_DIR)/$(TARGET) $(SRC_DIR)/symtab.c $(TEST_DIR)/testes.c -lm 
	$(BIN_DIR)/$(TARGET)


parser: $(PARSER)
	@mkdir -p $(BIN_DIR)
	$(BISON) -d $(PARSER) -o $(BIN_DIR)/$(TARGET).tab.c

scanner: $(SCANNER)
	@mkdir -p $(BIN_DIR)
	$(FLEX) -o $(BIN_DIR)/lex.yy.c $(SCANNER)

clean:
	rm -rf $(BIN_DIR)/*


# Meta para criar um backup compactado dos arquivos atuais
backup:
	@mkdir -p ./bkp
	tar -czvf ./bkp/`(date +%y-%m-%d-%H-%M-%S)`.tar.gz Makefile src/* include/*

