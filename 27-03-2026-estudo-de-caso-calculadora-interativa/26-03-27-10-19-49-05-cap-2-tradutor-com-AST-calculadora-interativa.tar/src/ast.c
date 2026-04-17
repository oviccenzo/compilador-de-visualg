#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include "ast.h"       // árvore sintática abstrata
#include "symtab.h"    // tabela de símbolos
#include "mathfuncs.h" // funções matemáticas pré-definidas

// cria um novo nó AST (genérico)
static ast_node *get_node() {

    ast_node *n = malloc(sizeof(ast_node));
    if( !n ) {
        fprintf(stderr,"Erro: falta de memoria\n");
        exit(1);
    }
    
    // n->type = NULL;;
    n->value = 0;
    n->name = NULL;
    n->left = NULL;
    n->right = NULL;
    n->args = NULL;
    n->arg_count = 0;

    return n;
}

// cria um novo nó para números
ast_node *ast_num(double v) {
    ast_node *n = get_node();
    n->type = AST_NUM;
    n->value = v;
    return n;
}

// cria um novo nó para variáveis (nós terminais)
ast_node *ast_var(char *name) {
    ast_node *n = get_node();
    n->type = AST_VAR;
    n->name = name;
    return n;
}

// cria um novo nó para um operador binário infixo
ast_node *ast_binary(ast_type type, ast_node *l, ast_node *r) {
    ast_node *n = get_node();
    n->type = type;
    n->left = l;
    n->right = r;
    return n;
}

// cria um novo nó para operador unário
ast_node *ast_unary(ast_type type, ast_node *child) {
    ast_node *n = get_node();
    n->type = type;
    n->left = child;
    return n;
}

// cria um novo nó para função pré-definida e seus parâmetros
ast_node *ast_func(char *name, ast_node **args, int argc) {
    ast_node *n = get_node();
    n->type = AST_FUNC;
    n->name = name;
    n->args = args;
    n->arg_count = argc;
    return n;
}

// cria um novo nó para atribuições
ast_node *ast_assign(char *name, ast_node *value) {
    ast_node *n = get_node();
    n->type = AST_ASSIGN;
    n->name = name;
    n->left = value;
    return n;
}

// avaliação da AST
double ast_eval(ast_node *n) {

    // observe o uso de "return" dentro das instruções "case", ao invés do "break"
    // tem o mesmo efeito, mas com o benefício de trazer simplicidade e objetividade
    // ao código-fonte, facilitando o procedimento de manutenção

    switch(n->type) {
        case AST_NUM:
            return n->value;

        case AST_ADD:
            return ast_eval(n->left) + ast_eval(n->right);

        case AST_SUB:
            return ast_eval(n->left) - ast_eval(n->right);

        case AST_MUL:
            return ast_eval(n->left) * ast_eval(n->right);

        case AST_POW:
        {
            double base = ast_eval(n->left);
            double expoente = ast_eval(n->right);
            return pow(base, expoente);
        }

        case AST_DIV:
        {
            double r = ast_eval(n->right);
            if(r == 0.0) {
                fprintf(stderr,"Erro: divisao por zero\n");
                return NAN;
            }
            return ast_eval(n->left) / r;
        }

        case AST_NEG:
            return -ast_eval(n->left);

        case AST_FACT:
        {
            double x = ast_eval(n->left);
            double r = (double) fatorial( x );
            return r;
        }

        case AST_VAR:
        {   if(!sym_is_set(n->name)) {
                fprintf(stderr,"Erro: variavel '%s' nao definida\n", n->name);
                return NAN;
            }
            return sym_value(n->name);
        }

        case AST_ASSIGN:
        {
            double v = ast_eval(n->left);
            sym_set(n->name, v);
            return v;
        }

        case AST_FUNC:
        {
            double *values = malloc(sizeof(double) * n->arg_count);

            for(int i=0;i<n->arg_count;i++)  {
                values[i] = ast_eval(n->args[i]);
            }

            double result = call_function(n->name, values, n->arg_count);

            free(values);
            return result;
        }
    }

    return 0;
}

// destruição da AST
void ast_free(ast_node *n) {

    if( !n ) return;

    switch(n->type) {
        case AST_NUM:
            break;

        case AST_VAR:
            free(n->name);
            break;

        case AST_FACT:
        case AST_NEG:
            ast_free(n->left);
            break;

        case AST_ADD:
        case AST_SUB:
        case AST_MUL:
        case AST_DIV:
        case AST_POW:
            ast_free(n->left);
            ast_free(n->right);
            break;

        case AST_ASSIGN:
            free(n->name);
            ast_free(n->left);
            break;

        case AST_FUNC:
            free(n->name);
            for(int i=0;i<n->arg_count;i++) ast_free(n->args[i]);
            free(n->args);
            break;
    }

    free(n);
}

// executa a chamada da função pré-definida
double call_function(const char *name, double *args, int argc) {
    
    math_func *f = find_func(name); // localiza a função na "tabela de funções"
    if( !f ) {
        fprintf(stderr,"Erro: funcao '%s' nao definida\n", name);
        return NAN;
    }
    
    if(argc != f->argc) {
        fprintf(stderr,"Erro: funcao '%s' espera %d argumentos, mas %d foram passados\n", name, f->argc, argc);
        return NAN;
    }
    
    if(f->argc == 1) {
        return f->fn.f1(args[0]);
    }
    return f->fn.f2(args[0], args[1]);
}

/* ------------------------------------------------------------------ */
/*  Impressão visual da AST (horizontal)                              */
/* ------------------------------------------------------------------ */

/*
 * Imprime a árvore na HORIZONTAL (raiz à esquerda, folhas à direita).
 * Algoritmo: travessia  direita → raiz → esquerda  com prefixos acumulados.
 */
static void ast_print_h(ast_node *n, FILE *out, const char *prefix, int is_right) {
    if (!n) return;

    const char *branch = is_right ? "┌── " : "└── ";

    char top_pfx[512];
    snprintf(top_pfx, sizeof(top_pfx), "%s%s", prefix, is_right ? "    " : "│   ");

    char bot_pfx[512];
    snprintf(bot_pfx, sizeof(bot_pfx), "%s%s", prefix, is_right ? "│   " : "    ");

    // Subárvore direita (imprime acima)
    if (n->type == AST_FUNC) {
        for (int i = 0; i < n->arg_count / 2; i++) {
               // Simplificação para funções: nâo há um conceito binário estrito, 
               // mas em h-print costuma-se dividir os argumentos entre "acima" e "abaixo"
        }
    }

    // Para binários puros
    if (n->right) ast_print_h(n->right, out, top_pfx, 1);

    // Nó atual
    fprintf(out, "%s%s", prefix, branch);
    switch (n->type) {
        case AST_NUM:    fprintf(out, "%.10g\n", n->value); break;
        case AST_VAR:    fprintf(out, "var(%s)\n", n->name); break;
        case AST_ASSIGN: fprintf(out, "atrib(%s)\n", n->name); break;
        case AST_ADD:    fprintf(out, "+\n"); break;
        case AST_SUB:    fprintf(out, "-\n"); break;
        case AST_MUL:    fprintf(out, "*\n"); break;
        case AST_DIV:    fprintf(out, "/\n"); break;
        case AST_POW:    fprintf(out, "^\n"); break;
        case AST_NEG:    fprintf(out, "-(unário)\n"); break;
        case AST_FACT:   fprintf(out, "!\n"); break;
        case AST_FUNC:   fprintf(out, "func(%s)\n", n->name); break;
    }

    // Subárvore esquerda (imprime abaixo)
    if (n->left) ast_print_h(n->left, out, bot_pfx, 0);

    // Argumentos de função (se houver)
    if (n->type == AST_FUNC && n->arg_count > 0) {
        for (int i = 0; i < n->arg_count; i++) {
             ast_print_h(n->args[i], out, bot_pfx, 0);
        }
    }
}

void ast_print(ast_node *n, FILE *out) {
    fprintf(out, "AST (horizontal):\n");
    ast_print_h(n, out, "", 0);
}

/* ========================================================== */
/* ========================================================== */

static void print_indent(int n) {
    for(int i=0;i<n;i++)
        printf("    ");
}

static const char *node_name(ast_type t) {
    switch(t)
    {
        case AST_NUM: return "NUM";
        case AST_VAR: return "VAR";
        case AST_ADD: return "ADD";
        case AST_SUB: return "SUB";
        case AST_MUL: return "MUL";
        case AST_DIV: return "DIV";
        case AST_NEG: return "NEG";
        case AST_ASSIGN: return "ASSIGN";
        case AST_FUNC: return "FUNC";
        case AST_POW: return "POW";
        case AST_FACT: return "FACT";
        default: return "UNKNOWN";
    }
}

static void ast_print_rec(ast_node *n, int depth) {
    if(!n) return;

    /* imprime ramo direito primeiro */
    if(n->right)
        ast_print_rec(n->right, depth + 1);

    print_indent(depth);

    switch(n->type)
    {
        case AST_NUM:
            printf("NUM(%.10g)\n", n->value);
            break;

        case AST_VAR:
            printf("VAR(%s)\n", n->name);
            break;

        case AST_FUNC:
            printf("FUNC(%s)\n", n->name);
            // mostra os argumentos da função na ordem declarada
            for(int i=n->arg_count-1;i>=0;i--)
                ast_print_rec(n->args[i], depth + 1);
            break;

        case AST_ASSIGN:
            printf("ASSIGN(%s)\n", n->name);
            ast_print_rec(n->left, depth + 1);
            return;

        default:
            printf("%s\n", node_name(n->type));
    }

    /* imprime ramo esquerdo */
    if(n->left)
        ast_print_rec(n->left, depth + 1);
}

void ast_mostrar(ast_node *node) {
    printf("\nAST\n");
    printf("===\n");
    ast_print_rec(node, 0);
    printf("\n");
}