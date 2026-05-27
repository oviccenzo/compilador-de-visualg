#ifndef AST_H
#define AST_H

#include <stdio.h>

typedef enum {
    AST_NUM,     // números
    AST_VAR,     // variáveis
    AST_ASSIGN,  // atribuição
    AST_ADD,     // operação de soma
    AST_SUB,     // operação de subtração
    AST_MUL,     // operação de multiplicação
    AST_DIV,     // operação de divisão
    AST_POW,     // operação de potencia inteira
    AST_NEG,     // oposto numérico ( -1 )
    AST_FUNC,    // função pré-definida
    AST_FACT     // calculo de fatorial
} ast_type;

// nó da árvore abstrata
typedef struct ast_node {

    ast_type type; // tipo desse nó

    // conteúdo do nó, dependendo do seu tipo
    double value;
    char *name;

    struct ast_node *left;   // sub-árvore à esquerda
    struct ast_node *right;  // sub-árvore à direita

    struct ast_node **args;  // lista de argumentos para função
    int arg_count;

} ast_node;

// ==========================================================
// manipulação da AST, segundo o tipo de regra gramatical
// ==========================================================

ast_node *ast_num(double v);
ast_node *ast_var(char *name);

ast_node *ast_assign(char *name, ast_node *value);

ast_node *ast_binary(ast_type type, ast_node *l, ast_node *r);
ast_node *ast_unary(ast_type type, ast_node *child);

ast_node *ast_func(char *name, ast_node **args, int argc);

// ==========================================================
// percurso na AST para executar alguma operação
// ==========================================================

// avaliar a expressão matemática 
double ast_eval(ast_node *node);

// destruir a AST
void   ast_free(ast_node *node);

// mostrar a AST na tela
void   ast_print(ast_node *node, FILE *out);

// mostrar a AST na tela
void ast_mostrar(ast_node *node);

// chamada de função pré-definida contida na AST durante a avaliação (ast_eval)
double call_function(const char *name, double *args, int argc);


#endif