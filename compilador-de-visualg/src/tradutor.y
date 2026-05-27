
 /* %define parse.error custom // tratamento de erro mais detalhado, com mensagens próprias */
%define parse.error verbose    // tratamento de erro mais detalhado
%locations                     // suporte regional

%{
    #define _USE_MATH_DEFINES

    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>
    #include <float.h>
    #include <string.h>

    #include "ast.h"       // definição da estrutura da árvore abstrata
    #include "symtab.h"    // definições da tabela de símbolos
    #include "mathfuncs.h" // tabela de funções e fatorial

    void show_help();

    /* Declarações de funções necessárias */
    void yyerror(const char *s);
    int yylex(void);
%}

// redefinição do repositório de atributos de tokens
%union {
    double dval;    // para atributos de números reais
    char  *sval;    // para atributos de strings
    ast_node *node; // o parser vai retornar um nó da AST
}

// associação de tipos para os símbolos não terminais da gramática
%type <node> exp fator termo potencia posfixo

// definição dos tokens que serão identificados pelo Flex
%token <dval> NUM  // token de números e seu atributo
%token <sval> ID   // token de identificador e seu atributo
%token <sval> FUNC // token para funções pre-definidas
%token EOL         // token para final de linha (tecla Enter)
%token VARS        // token para o comando de exibição da tabela de símbolos no console
%token SAIR
%token HELP

/* token definition */
%token ALGORITMO
%token INICIO 
%token VAR
%token FIMALGORITMO
%token INT
%token REAL
%token CARACTERE
%token LOGICO

/* Definição de precedência e associatividade (resolve ambiguidades) */
%left '+' '-'          // menor precedência
%left '*' '/'
%right '^'
%right '='             // atribuição (direita-associativa, estilo C)
%precedence UMINUS     // precedência usada para desambiguar o operador '-'

/* garante que strings de ID sejam liberadas se houver erro/descartes */
%destructor { free($$); } ID FUNC

%% /* ******************************************************** */

/* Regra inicial: permite múltiplas expressões separadas por quebra de linha */
calclist: /* vazio */
    | calclist line
    ;

line: EOL      { printf("> "); }

    | SAIR EOL { return 0; }

    | VARS EOL { symtab_print(stdout); // mostra o conteúdo da tabela de símbolos
                 printf("> ");
               }
    | HELP EOL { show_help();  // mostra as instruções de uso do interpretador
                 printf("> ");
               }
    | exp EOL  { double result = ast_eval($1); // avalia o resultado da expressão (contida na AST)
                 printf("Resultado = %.10g\n", result);
                 ast_print($1, stdout);  // mostra a AST
                //  ast_mostrar($1);        // mostra a AST (outro formato)
                 ast_free($1); // destroi toda a AST
                 printf("> "); // volta a mostrar o cursor na tela
               }
    | error EOL { yyerror("Linha invalida"); 
                  yyerrok; // permite continuar, mesmo com ocorrencia de erro
                  printf("> ");
                }
    ;

/* Regras de derivação para expressões aritméticas */

exp : fator            { $$ = $1; }
    | exp '+' fator    { $$ = ast_binary(AST_ADD, $1, $3); }
    | exp '-' fator    { $$ = ast_binary(AST_SUB, $1, $3); }
    | ID '=' exp       { $$ = ast_assign($1, $3); }
    ;

fator : potencia         { $$ = $1; }
    | fator '*' potencia { $$ = ast_binary(AST_MUL, $1, $3); }
    | fator '/' potencia { $$ = ast_binary(AST_DIV, $1, $3); }
    ;

potencia : posfixo         { $$ = $1; }
    | posfixo '^' potencia { $$ = ast_binary(AST_POW, $1, $3); }
    ;

posfixo: termo    { $$ = $1; }
    | posfixo '!' { $$ = ast_unary(AST_FACT, $1); }
    ;

termo : NUM                  { $$ = ast_num($1); }
    | '(' exp ')'            { $$ = $2; }
    | '-' termo %prec UMINUS { $$ = ast_unary(AST_NEG, $2); }
    | ID                     { $$ = ast_var($1); }
    
      // função com apenas um parametro
    | FUNC '(' exp ')' {
        ast_node **args = malloc(sizeof(ast_node*));
        if( !args ) {
            fprintf(stderr, "Erro: falta de memoria\n");
            exit(1);
        }
        args[0] = $3;
        $$ = ast_func($1, args, 1);
    }
    
      // função com dois parametros
    | FUNC '(' exp ',' exp ')' {
        ast_node **args = malloc(sizeof(ast_node*) * 2);
        if( ! args ) {
            fprintf(stderr,"Erro: falta de memoria\n");
            exit(1);
        }
        args[0] = $3;
        args[1] = $5;
        $$ = ast_func($1, args, 2);
    }
    ;

%% /* ******************************************************** */

void show_help() {
    puts("Comandos disponíveis:"); 
    puts("\tprint   ");
    puts("\ttabela  ");
    puts("\tvars    exibe a tabela de símbolos");
    puts("");
    puts("\tajuda");
    puts("\thelp    mostra essa tela");
    puts("");
    puts("\tsair    ");
    puts("\tquit    encerra a calculadora");             
}


/* Função chamada pelo Bison em caso de erro sintático */
void yyerror(const char *s) {
    fprintf(stderr, "Erro sintatico: %s\n", s);
    fflush(stderr);
}

/* Função principal (Ponto de entrada do programa) */
int main(void) {
    
    setvbuf(stdout, NULL, _IOLBF, 0);  // buffer por linha
    setvbuf(stderr, NULL, _IONBF, 0);  // sem buffer, para saída de erro

    // carregando as variáveis pré-definidas (constantes) na tabela de símbolos
    sym_set("pi", M_PI);
    sym_set("e",  M_E);

    puts("Calculadora Flex/Bison (double + tabela de simbolos)");
    puts("Exemplos:");
    puts("  x = 2.5");
    puts("  y = x * 4");
    puts("  y + 1");
    puts("Ctrl+D para sair.");
    
    printf("> ");
    yyparse();     // chamada ao parser....
    symtab_free(); // destruição da tabela de símbolos
    
    puts("Calculadora encerrada!");
    return 0;
}