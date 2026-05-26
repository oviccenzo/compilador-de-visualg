%{
    #include <stdlib.h>
    #include <string.h>
    #include "ast.h"              /* ast_node precisa ser conhecido antes do tab.h */
    #include "tradutor.tab.h" /* Arquivo de cabeçalho gerado pelo Bison com os tokens */
    
    static char *xstrdup(const char *s);
%}

/* Evita a necessidade de linkar com a biblioteca do Flex (-lfl) */
%option noyywrap
%option noinput
%option nounput
%option yylineno

%%

 /* Delimitacoes */
"algoritmo"			{ return ALGORITMO; }
"inicio"			{ return INICIO; }
"var"|"declare"		{ return VAR; }
"fimalgoritmo"		{ return FIMALGORITMO; }

 /* Tipos de dados */
"int" |
"inteiro"			{ return INT; }

"real" |
"numerico" 			{ return REAL; }

"caractere" |
"caracter" |
"literal" 	        { return CARACTERE; }

"logico"    		{ return LOGICO; }


/* ================================================================================= */
/* Adicionando as palavras reservada (a partir da palavras da reservada do visualg)  */
/* ================================================================================  */

"escreva"      {return ESCREVA;}
"escreval"     {return ESCREVAL;}
"leia"         {return LEIA;}
"entao"
"enquanto"
"faca"
"para"
"ate"
"passo"
"repita"


/* Operadores logico */
"e"
"ou"
"nao"
"<-"
"<>"
">="
"<="

"//"

 /* comandos para mostrar a tabela de símbolos no terminal */
"vars" | 
"tabela" | 
"print"     { return VARS; }

"sair" | 
"quit"      { return SAIR; }

"ajuda" |
"help"      { return HELP; }

"sin" | 
"cos" | 
"tan" | 
"asin" | 
"acos" | 
"atan" | 
"sinh" | 
"cosh" | 
"tanh" | 
"exp" | 
"round" | 
"sqrt" | 
"log" | 
"log2" | 
"log10" | 
"floor" | 
"ceil" | 
"abs" | 
"fabs" | 
"pow" | 
"atan2" |
"hypot" | 
"fmod" | 
"trunc" | 
"gamma"  | 
"lgamma" {
                  yylval.sval = xstrdup(yytext);
                  return FUNC;
                }

 /* números: inteiro, decimal, notação científica */
([0-9]+(\.[0-9]*)?|\.[0-9]+)([eE][+-]?[0-9]+)? {
                yylval.dval = strtod(yytext, NULL); // converte o lexema de string para double
                return NUM; 
            } 

 /* identificadores: variáveis */
[a-zA-Z_][a-zA-Z0-9_]*      {
                yylval.sval = xstrdup(yytext);  // copia o lexema para o atributo do token ID
                return ID;
            }

[ \t\r]+    { /* Ignora espaços em branco e tabulações */ }

 /* operadores e parênteses */
"," |
"=" | 
"+" | 
"-" | 
"*" | 
"^" | 
"!" | 
"/" | 
"(" | 
")"         { return yytext[0]; }

\n          { return EOL; } /* Fim de linha */

 /* .           { fprintf(stderr, "Caractere invalido: %s\n", yytext); }  */
.           { return yytext[0]; } // deixa o parser cuidar dos caracteres desconhecidos

%%

/* strdup portátil */
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
