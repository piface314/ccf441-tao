%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "defs.h"
    #include "symtable.h"
    #include "y.tab.h"
    void yyerror(const char *fmt);
    void yyerror_(const char *msg, YYLTYPE loc);
    int yylex();

    SymbolTable *root;
%}

/* Definições */
%locations

%union {
    int int_val;
    double real_val;
    char char_val;
    String string_val;
};

%token TRIG0 TRIG1 TRIG2 TRIG3 TRIG4 TRIG5 TRIG6 TRIG7
%token HEX00 HEX01 HEX02 HEX03 HEX04 HEX05 HEX06 HEX07
%token HEX08 HEX09 HEX10 HEX11 HEX12 HEX13 HEX14 HEX15
%token HEX16 HEX17 HEX18 HEX19 HEX20 HEX21 HEX22 HEX23
%token HEX24 HEX25 HEX26 HEX27 HEX28 HEX29 HEX30 HEX31
%token HEX32 HEX33 HEX34 HEX35 HEX36 HEX37 HEX38 HEX39
%token HEX40 HEX41 HEX42 HEX43 HEX44 HEX45 HEX46 HEX47
%token HEX48 HEX49 HEX50 HEX51 HEX52 HEX53 HEX54 HEX55
%token HEX56 HEX57 HEX58 HEX59 HEX60 HEX61 HEX62 HEX63

%token ENDL
%token WUJI YIN YANG
%token <int_val> INTEGER
%token <real_val> REAL
%token <char_val> CHAR
%token <string_val> STRING
%token <string_val> PRO_ID COM_ID
%token <string_val> SYM_ID_R8 SYM_ID_L7 SYM_ID_L6 SYM_ID_L5 SYM_ID_R5
%token <string_val> SYM_ID_N4 SYM_ID_L3 SYM_ID_L2
%token <string_val> SYM_ID_L1 SYM_ID_R1 SYM_ID_N1

%type <string_val> pro_id

%%
/* Regras / Produções */
program:
    module_decl top_stmts
    ;

module_decl:
    TRIG7 pro_id HEX56 exports ENDL
    |
    ;

exports:
    exports ',' export_id
    | export_id
    ;

export_id:
    COM_ID
    | PRO_ID
    | sym_id
    ;

top_stmts:
    top_stmts ENDL top_stmt
    | top_stmt
    ;

top_stmt:
    import
    | callable_def
    | def_type_params callable_def
    | var_def
    | type_def
    | type_alias
    |
    ;

stmts:
    stmts ENDL stmt
    | stmt
    ;

stmt:
    '{' stmts '}'
    | import
    | callable_def
    | def_type_params callable_def
    | var_def
    | type_def
    | type_alias
    ;
    /* 
    | call
    | if
    | match
    | while
    | repeat
    | free
    | break
    | continue
    | return
    | expr
    |
    ; */

import:
    TRIG6 pro_id
    | TRIG6 pro_id HEX51 exports
    | TRIG6 pro_id HEX54 PRO_ID
    ;

var_def:
    YIN COM_ID ':' type_id
    | YIN COM_ID ':' type_id '=' expr
    ;

def_type_params: HEX03 type_params;

callable_def: func_def | op_def | proc_def;

func_def: YANG COM_ID '(' param_list ')' ':' type_id '=' expr;

op_def: YANG sym_id_ '(' param ',' param ')' ':' type_id '=' expr;

proc_def: WUJI COM_ID '(' param_list ')' stmt;


type_def: TRIG0 PRO_ID type_param_list '=' constructors;
constructors: constructors ',' constructor | constructor;
constructor: PRO_ID '(' param_list ')' | PRO_ID;

param_list: params | ;
params: params ',' param | param;
param: COM_ID ':' type_id;

type_alias: HEX00 PRO_ID type_param_list '=' type_id;


expr: literal | com_id;


expr_9: literal
    | com_id
    | malloc
    | build
    | call
    | '(' expr ')'
    ;

assign: addr '=' expr;

addr: addr '[' expr ']'
    | addr '.' COM_ID
    | addr_0
    ;

addr_0: ptrs addr_1;
addr_1: com_id
    | '(' addr ')'
    ;

malloc: malloc_type type_id malloc_n
    | malloc_type expr malloc_n
    ;

malloc_type: TRIG1 | HEX12;
malloc_n: HEX11 INTEGER
    |
    ;

build: pro_id | pro_id '(' exprs ')';
call: com_id '(' expr_list ')';
expr_list: exprs 
    |
    ;
exprs: exprs ',' expr;

scope: scope PRO_ID '.' | ;
pro_id: scope PRO_ID {$$ = $2;};
com_id: scope COM_ID;
sym_id: scope sym_id_; 
sym_id_:
    SYM_ID_R8 | SYM_ID_L7 | SYM_ID_L6 | SYM_ID_L5
    | SYM_ID_R5 | SYM_ID_N4 | SYM_ID_L3 | SYM_ID_L2
    | SYM_ID_L1 | SYM_ID_R1 | SYM_ID_N1;

type_param_list: '(' type_params ')' | ;
type_params: type_params ',' type_id | type_id;
type_id:
    type_ptr pro_id type_param_list
    | type_ptr HEX57 type_param_list HEX57 type_id
    | type_ptr HEX57 type_param_list
    ;

type_ptr:
    type_ptr '@'
    | type_ptr '[' INTEGER ']'
    |
    ;

ptrs: ptrs '@' | ;

literal:
    INTEGER
    | REAL
    | CHAR
    | STRING
    ;

%%

/* Código auxiliar */

void yyerror(const char *msg) {
    fprintf(stderr, "%d:%d: syntax unbalance: %s\n", yylloc.first_line, yylloc.first_column, msg);
    exit(1);
}

void yyerror_(const char *msg, YYLTYPE loc) {
    fprintf(stderr, "%d:%d: syntax unbalance: %s\n", loc.first_line, loc.first_column, msg);
    exit(1);
}

void init_symbol_table() {
    
}

int main() {
    root = SymTable_new(NULL);
    init_symbol_table();
    yyparse();
    printf("\n\nTabela de Símbolos:\n");
    SymTable_show(root);
    return 0;
}

