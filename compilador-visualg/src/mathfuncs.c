#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <string.h>
#include "mathfuncs.h"

// tabela de funções pré-definidas
static math_func math_funcs[] = {
    // nome da função | quantos parametros | ponteiro para a funcao
    {"sin",  1, .fn.f1 = sin},
    {"cos",  1, .fn.f1 = cos},
    {"tan",  1, .fn.f1 = tan},

    {"asin", 1, .fn.f1 = asin},
    {"acos", 1, .fn.f1 = acos},
    {"atan", 1, .fn.f1 = atan},
    {"sinh", 1, .fn.f1 = sinh},
    {"cosh", 1, .fn.f1 = cosh},
    {"tanh", 1, .fn.f1 = tanh},
    {"exp",  1, .fn.f1 = exp},    // e^n
    {"round",1, .fn.f1 = round},

    {"sqrt", 1, .fn.f1 = sqrt},

    {"log",  1, .fn.f1 = log},
    {"log2", 1, .fn.f1 = log2},
    {"log10",1, .fn.f1 = log10},

    {"floor",1, .fn.f1 = floor},
    {"ceil", 1, .fn.f1 = ceil},

    {"abs",  1, .fn.f1 = fabs},
    {"fabs", 1, .fn.f1 = fabs},

    {"pow",  2, .fn.f2 = pow},
    {"atan2",2, .fn.f2 = atan2},
    {"hypot",2, .fn.f2 = hypot},
    {"fmod", 2, .fn.f2 = fmod},

    {"trunc", 1, .fn.f1 = trunc},
    {"gamma", 1, .fn.f1 = gamma},
    {"lgamma",1, .fn.f1 = lgamma},

    {NULL, 0, {0}} // marcador de fim de tabela
};


// encontra uma função
math_func *find_func(const char *name) {
    for (int i = 0; math_funcs[i].name != NULL; i++) {
        if (strcmp(name, math_funcs[i].name) == 0)
            return &math_funcs[i];
    }
    return NULL;
}

/* Calcula o fatorial de x com validações */
double fatorial(double x) {

    if (isnan(x)) return NAN;

    if (x < 0) {
        fprintf(stderr, "Erro: fatorial de numero negativo\n");
        return NAN;
    }

    /* exige inteiro */
    double xi = floor(x);
    if (fabs(x - xi) > 1e-12) {
        fprintf(stderr, "Erro: fatorial exige operando inteiro\n");
        return NAN;
    }

    if (xi > 170) {
        fprintf(stderr, "Erro: fatorial muito grande (maximo 170)\n");
        return NAN;
    }

    /* calcula */
    double r = 1.0;
    for (int i = 2; i <= (int)xi; i++) {
        r *= i;
        if (!isfinite(r)) {
            fprintf(stderr, "Erro: overflow no fatorial\n");
            return NAN;
        }
    }
    return r;
}
