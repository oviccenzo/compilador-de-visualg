TARGET = tradutor

SRC_DIR = .
INC_DIR = .

CC = gcc
FLEX = flex
BISON = bison

CFLAGS = -I$(INC_DIR) - Wall

BISON_OUT_C = ${SRC_DIR}/tradutor.tab.c
BISON_OUT_H = ${SRC_DIR}/tradutor.tab.h
FLEX_OUT_C = ${SRC_DIR}/lex.yy.c

OBJ = ${BISON_OUT_C} ${FLEX_OUT_C} 

all: ${TARGET}

$(TARGET) : ${BISON_OUT_C} ${FLEX_OUT_C}
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) -lm

$(BISON_OUT_C) $(BISON_OUT_H): $(SRC_DIR)/tradutor.y
	${BISON} -d -o $(BISON_OUT_C) $(SRC_DIR)/tradutor.y
	mv ${SRC_DIR}/tradutor.tab.h $(INC_DIR)/

$(FLEX_OUT_C) : $(SRC_DIR)/tradutor.l $(BISON_OUT_H)
	$(FLEX) -o $(FLEX_OUT_C) $(SRC_DIR)/tradutor.l

clean:
	rm -f $(TARGET) $(SRC_DIR)/*.tab.c $(SRC_DIR)lex.yy.c $(INC_DIR)/*.tab.h 