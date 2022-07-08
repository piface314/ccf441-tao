%{
#include <stdio.h>
void yyerror(char *msg);
int yylex();
%}

/* Definições */

%token INTEGER

%%
/* Regras / Produções */
expr: INTEGER

%%

/* Código auxiliar */

void yyerror(char *msg) {
    printf("%s\n", msg);
}

int main() {
    yyparse();
    return 0;
}

