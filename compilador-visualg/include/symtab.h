#ifndef SYMTAB_H

#define SYMTAB_H

#include <stdio.h>

typedef struct Symbol {
    char *name;            // representação do simbolo
    double value;          // valor do símbolo
    int is_set;            // 0 = não definido, 1 = definido
    struct Symbol *next;   // ponteiro para o próximo nó da lista de símbolos
} Symbol;

/* funções de manipulação */
Symbol * sym_get(const char *name);           /* retorna símbolo (cria se não existir) */
int      sym_is_set(const char *name);        /* verifica se o símbolo está definido ou não */
double   sym_value(const char *name);         /* obtem o valor de um símbolo (assume definido) */
void     sym_set(const char *name, double v); /* armazena o símbolo e seu valor */
void     symtab_print(FILE *out);             /* mostrar o conteúdo da tabela */
void     symtab_free(void);                   /* destroi a tabela de símbolo */

#ifdef TEST
unsigned symtab_test_hash(const char *s);
#endif

#endif
