#include <stdlib.h>
#include <string.h>
#include "symtab.h"  // inclui as definições expostas

#ifndef SYM_BUCKETS
#define SYM_BUCKETS 211   // quantidade de entradas na tabela
#endif

static Symbol * table[SYM_BUCKETS];

/*
   Calcula o código hash da string passada como parâmetro
   e retorna um valor entre 0 e SYM_BUCKETS
*/
static unsigned hash_str(const char *s) {
    #ifdef TEST
    const char * str = s;
    #endif
    
    unsigned h = 0u;
    while (*s) {
        h = h * 31u + (unsigned char)*s++;
    }
    
    #ifdef TEST
        printf("[DEBUG] \"%s\" --> %u mod %d = %d\n", str, h, SYM_BUCKETS, h % SYM_BUCKETS);
    #endif

    return h % SYM_BUCKETS;
}

/* Wrapper SOMENTE para testes */
#ifdef TEST
unsigned symtab_test_hash(const char *s) {
    return hash_str(s);
}
#endif

static char *xstrdup(const char *s) {
    size_t n = strlen(s) + 1;
    char *p = malloc(n);
    if (!p) { 
        fprintf(stderr, "Erro fatal: sem memoria\n"); 
        exit(1); 
    }
    memcpy(p, s, n);
    return p;
}

Symbol *sym_get(const char *name) {
    unsigned idx = hash_str(name);
    for (Symbol *p = table[idx]; p; p = p->next) {
        if (strcmp(p->name, name) == 0) return p;
    }
    Symbol *p = (Symbol*)calloc(1, sizeof(Symbol));
    if (!p) return NULL;
    p->name = xstrdup(name);
    p->value = 0.0;
    p->is_set = 0;
    p->next = table[idx];
    table[idx] = p;
    return p;
}

int sym_is_set(const char *name) {
    Symbol *s = sym_get(name);
    return s && s->is_set;
}

double sym_value(const char *name) {
    Symbol *s = sym_get(name);
    return s ? s->value : 0.0;
}

void sym_set(const char *name, double v) {
    Symbol *s = sym_get(name);
    if (!s) return;
    s->value = v;
    s->is_set = 1;
}

void symtab_print(FILE *out) {
    int printed = 0;
    fprintf(out, "---- Tabela de Simbolos ----\n");
    for (int i = 0; i < SYM_BUCKETS; i++) {
        for (Symbol *p = table[i]; p; p = p->next) {
            if (p->is_set) {
                fprintf(out, "%s = %.10g\n", p->name, p->value);
                printed = 1;
            }
        }
    }
    if (!printed) {
        fprintf(out, "(vazia)\n");
    }
    fprintf(out, "---------------------------\n");
}

void symtab_free(void) {
    for (int i = 0; i < SYM_BUCKETS; i++) {
        Symbol *p = table[i];
        while (p) {
            Symbol *n = p->next;
            free(p->name);
            free(p);
            p = n;
        }
        table[i] = NULL;
    }
}



