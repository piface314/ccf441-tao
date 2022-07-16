%{
    #include <stdio.h>
    #include <stdlib.h>
    #include "defs.h"
    #include "symtable.h"
    #include "y.tab.h"
    void yyerror(const char *fmt);
    void yyerror_(const char *msg, YYLTYPE loc);
    int yylex();
    Loc loc(YYLTYPE l);
    void install_params(List *params);

    int with_type_params = 0;
    SymbolTable *root = NULL;
    SymbolTable *env = NULL;
%}

/* Definições */
%locations

%union {
    int int_val;
    double real_val;
    char char_val;
    String string_val;
    ASTNode *node;
    List *list;
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

%nonassoc SYM_ID_N1
%right SYM_ID_R1
%left SYM_ID_L1
%left SYM_ID_L2
%left SYM_ID_L3
%nonassoc SYM_ID_N4
%right SYM_ID_R5
%left  SYM_ID_L5
%left  SYM_ID_L6
%left  SYM_ID_L7
%right SYM_ID_R8

%type <string_val> sym_id_
%type <node> scope pro_id com_id sym_id export_id type_id type_ptr
%type <node> param expr stmt top_stmt var_def callable_def func_def op_def proc_def
%type <node> type_def type_alias constr
%type <list> exports type_arg_list type_args type_param_list type_params param_list params
%type <list> constrs

%destructor { List_free($$); } <list>

%%
/* Regras / Produções */
program: module_decl { env = SymTable_new(env); } top_stmts ;

module_decl: TRIG7 pro_id HEX56 exports ENDL | ;

exports: exports ',' export_id { $$ = List_push($3, $1); }
       | export_id             { $$ = List_push($1, NULL); }
       ;

export_id: COM_ID  { $$ = Node_com_id(loc(@1), NULL, $1.s); }
         | PRO_ID  { $$ = Node_pro_id(loc(@1), NULL, $1.s); }
         | sym_id_ { $$ = Node_sym_id(loc(@1), NULL, $1.s); }
         ;

top_stmts:
    top_stmts ENDL top_stmt
    | top_stmt
    ;

top_stmt:
    import                         { $$ = NULL; }
    | callable_def                 { $$ = $1; }
    | def_type_params callable_def { $$ = NULL; }
    | var_def                      { $$ = $1; }
    | type_def                     { $$ = $1; }
    | type_alias                   { $$ = $1; }
    |                              { $$ = NULL; }
    ;

stmts:
    stmts ENDL stmt
    | stmt
    ;

stmt:
    '{' stmts '}'                  { $$ = NULL; }
    | top_stmt                     { $$ = $1; }
    ;

import: TRIG6 pro_id 
      | TRIG6 pro_id HEX51 exports
      | TRIG6 pro_id HEX54 PRO_ID 
      ;

var_def: YIN COM_ID ':' type_id {
            $$ = Node_yin(loc(@1), $2.s, $4, NULL);
            SymTable_install(SymTableEntry_new($$), env);
        }
       | YIN COM_ID ':' type_id '=' expr  {
            $$ = Node_yin(loc(@1), $2.s, $4, $6);
            SymTable_install(SymTableEntry_new($$), env);
        }
       ;

def_type_params: HEX03 type_params ENDL {
        with_type_params = 1;
        env = SymTable_new(env);
        for (List *c = $2; c; c = c->tail) {
            IdNode *id = (IdNode *)c->item;
            ASTNode *p = Node_def_type_alias(id->loc, id->id);
            SymTable_install(SymTableEntry_new(p), env);
            free(id);
        }
    };

callable_def: func_def { $$ = $1; }
            | op_def   { $$ = $1; }
            | proc_def { $$ = $1; }
            ;

func_def: YANG COM_ID '(' param_list ')' ':' type_id '=' <node>{
        ASTNode *t = Node_fun_type(loc(@4), NULL, List_push($7, $4));
        $$ = Node_yang(loc(@1), $2.s, t, NULL);
        if (with_type_params) {
            SymTable_install(SymTableEntry_new($$), env->parent);
        } else {
            SymTable_install(SymTableEntry_new($$), env);
            env = SymTable_new(env);
        }
        install_params($4);
    }[def] expr[body] {
        env = env->parent, with_type_params = 0;
        $def->def_node.body = $body;
        $$ = $def;
    } ;

op_def: YANG sym_id_ '(' param ',' param ')' ':' type_id '=' <node>{
        List *params = List_push($6, List_push($4, NULL));
        List *p_ret = List_push($9, params);
        ASTNode *t = Node_fun_type(loc(@4), NULL, p_ret);
        $$ = Node_yang(loc(@1), $2.s, t, NULL);
        if (with_type_params) {
            SymTable_install(SymTableEntry_new($$), env->parent);
        } else {
            SymTable_install(SymTableEntry_new($$), env);
            env = SymTable_new(env);
        }
        install_params(params);
        List_free(p_ret);
    }[def] expr[body] {
        env = env->parent, with_type_params = 0;
        $def->def_node.body = $body;
        $$ = $def;
    } ;

proc_def: WUJI COM_ID '(' param_list ')' <node>{
        ASTNode *t = Node_proc_type(loc(@4), NULL, $4);
        $$ = Node_wuji(loc(@1), $2.s, t, NULL);
        if (with_type_params) {
            SymTable_install(SymTableEntry_new($$), env->parent);
        } else {
            SymTable_install(SymTableEntry_new($$), env);
            env = SymTable_new(env);
        }
        install_params($4);
    }[def] stmt[body] {
        env = env->parent, with_type_params = 0;
        $def->def_node.body = $body;
        $$ = $def;
    } ;


type_def: TRIG0 PRO_ID type_param_list '=' <node>{
        ASTNode *decl = Node_type_decl(loc(@2), $2.s, $3);
        $$ = Node_type_def(loc(@1), decl);
        SymTable_install(SymTableEntry_new($$), env);
    }[def] constrs[cs] {
        for (List *c = $cs; c; c = c->tail) {
            ASTNode *t = (ASTNode *)$def->type_def_node.decl;
            ASTNode *p = Node_adj_constr(t, (ASTNode *)c->item);
            SymTable_install(SymTableEntry_new(p), env);
        }
        $$ = Node_adj_type_def($cs, $def);
    };
constrs: constrs ',' constr { $$ = List_push($3, $1); }
       | constr             { $$ = List_push($1, NULL); }
       ;
constr: PRO_ID '(' param_list ')' { $$ = Node_constructor(loc(@1), $1.s, $3); }
      | PRO_ID                    { $$ = Node_constructor(loc(@1), $1.s, NULL); }
      ;

param_list: params { $$ = $1; } | { $$ = NULL; } ;
params: params ',' param { $$ = List_push($3, $1); }
      | param            { $$ = List_push($1, NULL); }
      ;
param: COM_ID ':' type_id { $$ = Node_yin(loc(@1), $1.s, $3, NULL); } ;

type_alias: HEX00 PRO_ID type_param_list '=' type_id {
        ASTNode *decl = Node_type_decl(loc(@2), $2.s, $3);
        $$ = Node_type_alias(loc(@1), decl, $5);
        SymTable_install(SymTableEntry_new($$), env);
    };

expr: literal {$$ = NULL;}
    | com_id {$$ = NULL;}
    ;

scope: scope PRO_ID '.' { $$ = Node_scope(loc(@2), $1, $2.s); } | {$$ = NULL;} ;
pro_id: scope PRO_ID  { $$ = Node_pro_id(loc(@2), $1, $2.s); };
com_id: scope COM_ID  { $$ = Node_com_id(loc(@2), $1, $2.s); };
sym_id: scope sym_id_ { $$ = Node_sym_id(loc(@2), $1, $2.s); }; 
sym_id_: SYM_ID_R8    { $$ = $1; }
       | SYM_ID_L7    { $$ = $1; }
       | SYM_ID_L6    { $$ = $1; }
       | SYM_ID_L5    { $$ = $1; }
       | SYM_ID_R5    { $$ = $1; }
       | SYM_ID_N4    { $$ = $1; }
       | SYM_ID_L3    { $$ = $1; }
       | SYM_ID_L2    { $$ = $1; }
       | SYM_ID_L1    { $$ = $1; }
       | SYM_ID_R1    { $$ = $1; }
       | SYM_ID_N1    { $$ = $1; }
       ;

type_param_list: '(' type_params ')' { $$ = $2; } | { $$ = NULL; } ;
type_params: type_params ',' PRO_ID  { $$ = List_push(Node_pro_id(loc(@3), NULL, $3.s), $1); }
         | PRO_ID                    { $$ = List_push(Node_pro_id(loc(@1), NULL, $1.s), NULL); }
         ;

type_arg_list: '(' type_args ')' { $$ = $2; } | { $$ = NULL; } ;
type_args: type_args ',' type_id { $$ = List_push($3, $1); }
         | type_id               { $$ = List_push($1, NULL); }
         ;
type_id:
    type_ptr pro_id type_arg_list          { $$ = Node_var_type(loc(@2), $1, $2, $3);  }
    | type_ptr type_arg_list HEX57 type_id { $$ = Node_fun_type(loc(@2), $1, List_push($4, $2)); }
    | type_ptr type_arg_list HEX59         { $$ = Node_proc_type(loc(@2), $1, $2); }
    ;

type_ptr:
    type_ptr '@'               { $$ = Node_ptr_type(loc(@2), $1, 0); }
    | type_ptr '[' INTEGER ']' { $$ = Node_ptr_type(loc(@2), $1, $3); }
    |                          { $$ = NULL; }
    ;

literal:
    INTEGER
    | REAL
    | CHAR
    | STRING
    ;

%%

/* Código auxiliar */

void yyerror(const char *msg) {
    printf("\n\nTabela de Símbolos:\n");
    SymTable_show(root);
    fprintf(stdout, "\n\n%d:%d: syntax unbalance: %s\n", yylloc.first_line, yylloc.first_column, msg);
    exit(1);
}

void yyerror_(const char *msg, YYLTYPE loc) {
    printf("\n\nTabela de Símbolos:\n");
    SymTable_show(root);
    fprintf(stdout, "\n\n%d:%d: syntax unbalance: %s\n", loc.first_line, loc.first_column, msg);
    exit(1);
}

void init_symbol_table() {
    root = SymTable_new(NULL);
    // adicionar os tipos predefinidos
    // Any e Null
    // adicionar os operadores predefinidos
    // adicionar as funções predefinidas (se tiver alguma)
}

Loc loc(YYLTYPE l) {
    Loc l_ = { .line = l.first_line, .col = l.first_column };
    return l_;
}

void install_params(List *params) {
    for (List *c = params; c; c = c->tail) {
        ASTNode *p = (ASTNode *)c->item;
        SymTable_install(SymTableEntry_new(p), env);
    }   
}

int main() {
    init_symbol_table();
    env = root;
    yyparse();
    printf("\n\nTabela de Símbolos:\n");
    SymTable_show(root);
    printf("\nSyntax is balanced.\n");
    return 0;
}

