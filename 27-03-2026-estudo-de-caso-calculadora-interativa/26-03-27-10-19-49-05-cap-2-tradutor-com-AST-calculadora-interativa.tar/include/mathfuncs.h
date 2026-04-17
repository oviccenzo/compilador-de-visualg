#ifndef MATHFUNCS_H
#define MATHFUNCS_H

#include <stdio.h>

/* Tabela de mapeamento de funções matemáticas pré-definidas */
typedef struct {
    const char *name;        // nome da função matemática
    int argc;                // aridade da função
    union {
        double (*f1)(double);          // ponteiro para função com aridade 1
        double (*f2)(double, double);  // ponteiro para função com aridade 2
    } fn;
} math_func;

math_func *find_func(const char *name);
double     fatorial(double x);

#endif
