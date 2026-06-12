%{
    #define _USE_MATH_DEFINES
    #include <stdio.h>
    #include <stdlib.h>
    #include <math.h>
    #include <float.h>
    #include <string.h>
    #include "ast.h"       
    #include "symtab.h"    
    #include "mathfuncs.h" 

    void show_help();
    void yyerror(const char *s);
    int yylex(void);
%}

%union {
    double dval;    
    char  *sval;    
    ast_node *node; 
}

%token <dval> NUM  
%token <sval> ID   
%token <sval> FUNC 
%token EOL VARS SAIR HELP
%token ALGORITMO INICIO VAR FIMALGORITMO
%token INT REAL CARACTERE LOGICO
%token UMINUS

/* === TOKENS DO VISUALG === */
%token ESCREVA ESCREVAL LEIA ENTAO ENQUANTO FACA PARA ATE PASSO REPITA
%token E OU NAO ATRIBUICAO DIFERENTE MAIOR_IGUAL MENOR_IGUAL

%type <node> exp fator potencia posfixo termo

%%

calclist: 
        | calclist line
        ;

line: EOL       { printf("> "); }
    | SAIR EOL  { return 0; }
    | VARS EOL  { symtab_print(stdout); printf("> "); }
    | HELP EOL  { show_help(); printf("> "); }
    | exp EOL   { double result = ast_eval($1); printf("Resultado = %.10g\n", result); ast_print($1, stdout); ast_free($1); printf("> "); }
    | error EOL { yyerror("Linha invalida"); yyerrok; printf("> "); }
    ;

exp: fator                  { $$ = $1; }
   | exp '+' fator          { $$ = ast_binary(AST_ADD, $1, $3); }
   | exp '-' fator          { $$ = ast_binary(AST_SUB, $1, $3); }
   | ID '=' exp             { $$ = ast_assign($1, $3); }
   ;

fator: potencia             { $$ = $1; }
     | fator '*' potencia   { $$ = ast_binary(AST_MUL, $1, $3); }
     | fator '/' potencia   { $$ = ast_binary(AST_DIV, $1, $3); }
     ;

potencia: posfixo               { $$ = $1; }
        | posfixo '^' potencia  { $$ = ast_binary(AST_POW, $1, $3); }
        ;

posfixo: termo              { $$ = $1; }
       | posfixo '!'        { $$ = ast_unary(AST_FACT, $1); }
       ;

termo: NUM                  { $$ = ast_num($1); }
     | '(' exp ')'          { $$ = $2; }
     | '-' termo %prec UMINUS { $$ = ast_unary(AST_NEG, $2); }
     | ID                   { $$ = ast_var($1); }
     | FUNC '(' exp ')'     { 
                                ast_node **args = malloc(sizeof(ast_node*));
                                if(!args) { fprintf(stderr, "Erro: falta de memoria\n"); exit(1); }
                                args[0] = $3;
                                $$ = ast_func($1, args, 1);
                            }
     | FUNC '(' exp ',' exp ')' {
                                ast_node **args = malloc(sizeof(ast_node*) * 2);
                                if(!args) { fprintf(stderr,"Erro: falta de memoria\n"); exit(1); }
                                args[0] = $3; args[1] = $5;
                                $$ = ast_func($1, args, 2);
                            }
     ;

%%

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

void yyerror(const char *s) {
    fprintf(stderr, "Erro sintatico: %s\n", s);
    fflush(stderr);
}

int main(void) {
    setvbuf(stdout, NULL, _IOLBF, 0);  
    setvbuf(stderr, NULL, _IONBF, 0);  

    sym_set("pi", M_PI);
    sym_set("e",  M_E);

    puts("Calculadora Flex/Bison (VisualG Mode)");
    printf("> ");
    yyparse();     
    symtab_free(); 
    
    puts("Calculadora encerrada!");
    return 0;
}

