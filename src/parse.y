
%code requires {
    #include <iostream>
    #include <sstream>
    #include "ast.hpp"
}

%{
    #include "symtable.hpp"
    #include "parser.hpp"
    #define VPUSH(D,V,I) D = V; if(I)V->push_back(I)
    #define VINIT(D,I)   D = new std::vector<ASTNode*>(); if(I)D->push_back(I)
    #define VEMPTY       new std::vector<ASTNode*>()
    #define ENVPUSH env = new SymTable(env)
    #define ENVPOP  SymTable *t = env; env = env->prev; delete t

    void yyerror(const char *fmt);
    void yyerror_(const char *msg, YYLTYPE loc);
    int yylex();

    SymTable *env = NULL;
    ASTNode *ast = NULL;

    bool with_type_params = false;
    std::vector<std::string> errors;
    std::vector<ASTNode*> empty;
%}

/* Definições */
%locations

%union {
    int int_val;
    double real_val;
    char char_val;
    std::string *string_val;
    ASTNode *node;
    CallableNode *cnode;
    std::vector<ASTNode *> *list;
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

%token PREFIX

%token ENDL
%token WUJI YIN YANG
%token <int_val> INTEGER
%token <real_val> REAL
%token <char_val> CHAR
%token <string_val> STRING
%token <string_val> PRO_ID QPRO_ID COM_ID QCOM_ID
%token <string_val> SYM_ID_R8 QSYM_ID_R8 SYM_ID_L7 QSYM_ID_L7
%token <string_val> SYM_ID_L6 QSYM_ID_L6 SYM_ID_L5 QSYM_ID_L5
%token <string_val> SYM_ID_R5 QSYM_ID_R5 SYM_ID_N4 QSYM_ID_N4
%token <string_val> SYM_ID_L3 QSYM_ID_L3 SYM_ID_L2 QSYM_ID_L2
%token <string_val> SYM_ID_L1 QSYM_ID_L1 SYM_ID_R1 QSYM_ID_R1
%token <string_val> SYM_ID_N1 QSYM_ID_N1

%right '='
%nonassoc SYM_ID_N1 QSYM_ID_N1 
%right SYM_ID_R1 QSYM_ID_R1
%left SYM_ID_L1 QSYM_ID_L1
%left SYM_ID_L2 QSYM_ID_L2
%left SYM_ID_L3 QSYM_ID_L3
%nonassoc SYM_ID_N4 QSYM_ID_N4
%right SYM_ID_R5 QSYM_ID_R5
%left  SYM_ID_L5 QSYM_ID_L5
%left  SYM_ID_L6 QSYM_ID_L6
%left  SYM_ID_L7 QSYM_ID_L7
%nonassoc PREFIX
%right SYM_ID_R8 QSYM_ID_R8

%type <string_val> pro_id com_id sym_id_
%type <node> module_decl export type_id
%type <node> param expr stmt top_stmt var_def import
%type <node> type_def type_alias constr decons
%type <node> while repeat step free break continue return
%type <node> assign addr if else literal match case default
%type <node> malloc malloc_n build call
%type <cnode> callable_def func_def op_def proc_def
%type <list> exports type_arg_list type_args type_param_list type_params param_list params
%type <list> constrs com_id_list com_ids call_type_params top_stmts stmts elif cases
%type <list> expr_list exprs type_ptr

%%
/* Regras / Produções */

// Regra inicial
program: module_decl { ENVPUSH; } top_stmts { ast = new ProgramNode($1,*$3); delete $3; } ;

// Declaração de módulo
module_decl: TRIG7 pro_id HEX56 exports ENDL { $$ = new ExportNode(*$2,*$4); delete $2; delete $4; }
           | TRIG7 error HEX56 exports ENDL { yyerror_("expected a proper identifier", @2); }
           | TRIG7 pro_id error exports ENDL { yyerror_("expected |||:::", @3); }
           | { $$ = NULL; }
           ;

exports: exports ',' export { VPUSH($$,$1,$3); }
       | export             { VINIT($$,$1); }
       ;

export: COM_ID  { $$ = new IDNode(*$1); delete $1; }
      | PRO_ID  { $$ = new IDNode(*$1); delete $1; }
      | sym_id_ { $$ = new IDNode(*$1); delete $1; }
      | error   { $$ = NULL; yyerror_("expected a nonqualified identifier", @1); }
      ;

// Comandos que podem aparecer no escopo do topo
top_stmts: top_stmts ENDL top_stmt { VPUSH($$,$1,$3); }
         | top_stmt                { VINIT($$,$1); }
         ;

top_stmt: import                        { $$ = $1; }
        | type_def                      { $$ = $1; }
        | type_alias                    { $$ = $1; }
        | callable_def                  { $$ = $1; }
        | call_type_params callable_def { $2->set_type_params(*$1); delete $1; $$ = $2; }
        | var_def                       { $$ = $1; }
        |                               { $$ = NULL; }
        ;

// Importação de outros módulos
import: TRIG6 pro_id                { $$ = new ImportNode(*$2, empty, ""); delete $2; }
      | TRIG6 pro_id HEX51 exports  { $$ = new ImportNode(*$2, *$4, ""); delete $2; delete $4; }
      | TRIG6 pro_id HEX54 PRO_ID   { $$ = new ImportNode(*$2, empty, *$4); delete $2; delete $4; }
      ;


// Definição de novos tipos
type_def: TRIG0 PRO_ID { ENVPUSH; } type_param_list '=' <node>{
        $$ = new TypeDefNode(*$2, *$4); delete $4;
        env->prev->install(*$2, SymTableEntry(Loc(@2), $$)); delete $2;
    }[def] constrs[cs] {
        ENVPOP;
        dynamic_cast<TypeDefNode*>($def)->add_constrs(*$cs);
        delete $cs;
        $$ = $def;
    };

constrs: constrs ',' constr { VPUSH($$,$1,$3); }
       | constr             { VINIT($$,$1); }
       ;
constr: PRO_ID '(' param_list ')' {
            $$ = new ConstrNode(*$1, *$3); delete $3;
            env->prev->install(*$1, SymTableEntry(Loc(@1), $$)); delete $1;
        }
      | PRO_ID {
            $$ = new ConstrNode(*$1, empty);
            env->prev->install(*$1, SymTableEntry(Loc(@1), $$)); delete $1;
        }
      ;

// Declaração de tipos, dá um novo nome a um tipo já existente
type_alias: HEX00 PRO_ID { ENVPUSH; } type_param_list '=' type_id {
        ENVPOP;
        $$ = new TypeAliasNode(*$2, *$4, $6); delete $4;
        env->install(*$2, SymTableEntry(Loc(@2), $$)); delete $2;
    };

type_param_list: '(' type_params ')' { $$ = $2; } | { $$ = VEMPTY; } ;
type_params: type_params ',' PRO_ID {
                auto node = new TypeParamNode(*$3);
                env->install(*$3, SymTableEntry(Loc(@3), node)); delete $3;
                VPUSH($$,$1,node);
            }
           | PRO_ID                 {
                auto node = new TypeParamNode(*$1);
                env->install(*$1, SymTableEntry(Loc(@1), node)); delete $1;
                VINIT($$,node);
            }
           ;

// Parâmetros de tipo temporários para funções e procedimentos
call_type_params: HEX03 { ENVPUSH; } type_params {
        with_type_params = true;
        $$ = $3;
    };

// Definição de funções e procedimentos
callable_def: func_def { $$ = $1; }
            | op_def   { $$ = $1; }
            | proc_def { $$ = $1; }
            ;

// Definição de função
func_def: YANG COM_ID {
            if (!with_type_params) ENVPUSH;
        } '(' param_list ')' ':' type_id '=' <cnode>{
        with_type_params = false;
        $$ = new YangNode(*$2, *$5, $8); delete $5;
        env->prev->install(*$2, SymTableEntry(Loc(@2), $$)); delete $2;
    }[def] expr[body] { ENVPOP; $def->set_body($body); $$ = $def; } ;

// Definição de operador (função com identificador de símbolo)
op_def: YANG sym_id_ {
            if (!with_type_params) ENVPUSH;
        } '(' param ',' param ')' ':' type_id '=' <cnode>{
        with_type_params = false;
        std::vector<ASTNode*> params;
        params.push_back($5);
        params.push_back($7);
        $$ = new YangNode(*$2, params, $10);
        env->prev->install(*$2, SymTableEntry(Loc(@2), $$)); delete $2;
    }[def] expr[body] { ENVPOP; $def->set_body($body); $$ = $def; } ;

// Definição de procedimento
proc_def: WUJI COM_ID {
            if (!with_type_params) ENVPUSH;
        } '(' param_list ')' <cnode>{
        with_type_params = false;
        $$ = new WujiNode(*$2, *$5); delete $5;
        env->prev->install(*$2, SymTableEntry(Loc(@2), $$)); delete $2;
    }[def] stmt[body] { ENVPOP; $def->set_body($body); $$ = $def; } ;

param_list: params { $$ = $1; }| { $$ = VEMPTY; } ;
params: params ',' param { VPUSH($$,$1,$3); }
      | param            { VINIT($$,$1); }
      ;
param: COM_ID ':' type_id {
        $$ = new ParamNode(*$1, $3);
        env->install(*$1, SymTableEntry(Loc(@1), $$)); delete $1;
    } ;

// Definição de variável
var_def: YIN COM_ID ':' type_id {
            $$ = new YinNode(*$2, $4, NULL);
            env->install(*$2, SymTableEntry(Loc(@2), $$)); delete $2;
        }
       | YIN COM_ID ':' type_id '=' expr  {
            $$ = new YinNode(*$2, $4, $6);
            env->install(*$2, SymTableEntry(Loc(@2), $$)); delete $2;
        }
       ;

// Comandos gerais
stmts: stmts ENDL stmt { VPUSH($$,$1,$3); }
     | stmt            { VINIT($$,$1); }
     ;

stmt: top_stmt { $$ = $1; }
    | while    { $$ = $1; }
    | repeat   { $$ = $1; }
    | free     { $$ = $1; }
    | break    { $$ = $1; }
    | continue { $$ = $1; }
    | return   { $$ = $1; }
    | expr     { $$ = $1; }
    ;

// Comandos de repetição
while: TRIG3 expr step HEX31 stmt { $$ = new WhileNode($2, $3, $5); };
repeat: HEX27 stmt HEX25 expr step { $$ = new RepeatNode($2, $4, $5); };
step: HEX28 stmt { $$ = $2; } | { $$ = NULL; } ;

// Controle de fluxo
break: HEX30 { $$ = new BreakNode(); } ;
continue: HEX26 { $$ = new ContinueNode(); } ;
return: HEX62      { $$ = new ReturnNode(NULL); }
      | HEX62 expr { $$ = new ReturnNode($2); }
      ;

// Liberação de memória
free: TRIG4 addr { $$ = new FreeNode($2); } ;

// Expressões
expr: '{' { ENVPUSH; } stmts '}' { ENVPOP; $$ = new BlockNode(*$3); delete $3; }
    | assign                        { $$ = $1; }
    | match                         { $$ = $1; }
    | if                            { $$ = $1; }
    | expr SYM_ID_L1 expr           { $$ = new BinaryOpNode(*$2, $1, $3); delete $2; }
    | expr QSYM_ID_L1 expr          { $$ = new BinaryOpNode(*$2, $1, $3); delete $2; }
    | expr SYM_ID_N1 expr           { $$ = new BinaryOpNode(*$2, $1, $3); delete $2; }
    | expr QSYM_ID_N1 expr          { $$ = new BinaryOpNode(*$2, $1, $3); delete $2; }
    | expr SYM_ID_R1 expr           { $$ = new BinaryOpNode(*$2, $1, $3); delete $2; }
    | expr QSYM_ID_R1 expr          { $$ = new BinaryOpNode(*$2, $1, $3); delete $2; }
    | expr SYM_ID_L2 expr           { $$ = new BinaryOpNode(*$2, $1, $3); delete $2; }
    | expr QSYM_ID_L2 expr          { $$ = new BinaryOpNode(*$2, $1, $3); delete $2; }
    | expr SYM_ID_L3 expr           { $$ = new BinaryOpNode(*$2, $1, $3); delete $2; }
    | expr QSYM_ID_L3 expr          { $$ = new BinaryOpNode(*$2, $1, $3); delete $2; }
    | expr SYM_ID_N4 expr           { $$ = new BinaryOpNode(*$2, $1, $3); delete $2; }
    | expr QSYM_ID_N4 expr          { $$ = new BinaryOpNode(*$2, $1, $3); delete $2; }
    | expr SYM_ID_R5 expr           { $$ = new BinaryOpNode(*$2, $1, $3); delete $2; }
    | expr QSYM_ID_R5 expr          { $$ = new BinaryOpNode(*$2, $1, $3); delete $2; }
    | expr SYM_ID_L5 expr           { $$ = new BinaryOpNode(*$2, $1, $3); delete $2; }
    | expr QSYM_ID_L5 expr          { $$ = new BinaryOpNode(*$2, $1, $3); delete $2; }
    | expr SYM_ID_L6 expr           { $$ = new BinaryOpNode(*$2, $1, $3); delete $2; }
    | expr QSYM_ID_L6 expr          { $$ = new BinaryOpNode(*$2, $1, $3); delete $2; }
    | expr '-' expr %prec SYM_ID_L6 { $$ = new BinaryOpNode("-", $1, $3); }
    | expr SYM_ID_L7 expr           { $$ = new BinaryOpNode(*$2, $1, $3); delete $2; }
    | expr QSYM_ID_L7 expr          { $$ = new BinaryOpNode(*$2, $1, $3); delete $2; }
    | expr SYM_ID_R8 expr           { $$ = new BinaryOpNode(*$2, $1, $3); delete $2; }
    | expr QSYM_ID_R8 expr          { $$ = new BinaryOpNode(*$2, $1, $3); delete $2; }
    | '@' expr %prec PREFIX         { $$ = new UnaryOpNode("@", $2); }
    | '~' expr %prec PREFIX         { $$ = new UnaryOpNode("~", $2); }
    | '!' expr %prec PREFIX         { $$ = new UnaryOpNode("!", $2); }
    | '-' expr %prec PREFIX         { $$ = new UnaryOpNode("-", $2); }
    | malloc                        { $$ = $1; }
    | build                         { $$ = $1; }
    | call                          { $$ = $1; }
    | addr                          { $$ = $1; }
    | literal                       { $$ = $1; }
    | '(' expr ')'                  { $$ = $2; }
    ;

// Expressão de atribuição
assign: addr '=' expr   { $$ = new AssignNode($1, $3); };
addr: addr '[' expr ']' { $$ = new AddressNode($1, $3); }
    | addr '@'          { $$ = new AddressNode($1); }
    | addr '.' COM_ID   { $$ = new AccessNode($1, *$3); delete $3; }
    | com_id            { $$ = new IDNode(*$1); delete $1; }
    ;

// Expressão condicional
if: TRIG2 expr HEX20 stmt elif else {
        $5->insert($5->begin(), new IfNode($2,$4));
        $5->push_back($6);
        for (auto it = $5->begin(); it != $5->end() - 1; ++it)
            dynamic_cast<IfNode*>(*it)->set_else(*(it + 1));
        $$ = *$5->begin();
        delete $5;
    }
elif: elif HEX18 expr HEX20 stmt { VPUSH($$,$1,new IfNode($3,$5)); }
    | { $$ = VEMPTY; } ;
else: HEX19 stmt { $$ = $2; } | { $$ = NULL; } ;

// Expressão de casamento de tipo
match: TRIG5 expr cases default { $$ = new MatchNode($2, *$3, $4); delete $3; };
cases: cases case  { VPUSH($$,$1,$2); }
     | case        { VINIT($$,$1); } ;
case: HEX47 literal HEX42 stmt { $$ = new CaseNode(true, $2, $4); }
    | HEX47 decons { ENVPUSH; } HEX42 stmt { ENVPOP; $$ = new CaseNode(false, $2, $5); }
    ;
default: HEX44 stmt { $$ = $2; } | { $$ = NULL; } ;
decons: pro_id '(' com_id_list ')' { $$ = new DeconsNode(*$1, *$3); delete $3; delete $1; };
com_id_list: com_ids { $$ = $1; } | { $$ = VEMPTY; } ;
com_ids: com_ids ',' COM_ID { VPUSH($$,$1,new IDNode(*$3)); delete $3; }
       | COM_ID             { VINIT($$,new IDNode(*$1)); delete $1; }
       ;

// Expressão de alocação de memória
malloc: TRIG1 type_id malloc_n { $$ = new MallocNode($2, NULL, $3); }
      | HEX13 expr malloc_n    { $$ = new MallocNode(NULL, $2, $3); }
      ;
malloc_n: HEX11 expr { $$ = $2; } | { $$ = NULL; } ;

// Chamada de construtor de tipo
build: pro_id                   { $$ = new CallNode(*$1, empty); delete $1; }
     | pro_id '(' expr_list ')' { $$ = new CallNode(*$1, *$3); delete $1; delete $3; }
     ;

// Chamada de função ou procedimento
call: com_id '(' expr_list ')' { $$ = new CallNode(*$1, *$3); delete $1; delete $3; } ;

expr_list: exprs { $$ = $1; } | { $$ = VEMPTY; } ;
exprs: exprs ',' expr { VPUSH($$,$1,$3); }
     | expr           { VINIT($$,$1); }
     ;

// Identificador de tipo
type_id: type_ptr pro_id type_arg_list {
            $1->push_back(new VarTypeNode(*$2, *$3)); delete $2; delete $3;
            for (auto it = $1->begin(); it != $1->end()-1; ++it)
                dynamic_cast<PtrTypeNode*>(*it)->set_type(*(it+1));
            $$ = *$1->begin(); delete $1;
        }
       | type_ptr type_arg_list HEX57 type_id {
            $1->push_back(new FunTypeNode(*$2, $4)); delete $2;
            for (auto it = $1->begin(); it != $1->end()-1; ++it)
                dynamic_cast<PtrTypeNode*>(*it)->set_type(*(it+1));
            $$ = *$1->begin(); delete $1;
        }
       | type_ptr type_arg_list HEX59 {
            $1->push_back(new ProcTypeNode(*$2)); delete $2;
            for (auto it = $1->begin(); it != $1->end()-1; ++it)
                dynamic_cast<PtrTypeNode*>(*it)->set_type(*(it+1));
            $$ = *$1->begin(); delete $1;
        }
       ;

type_ptr: type_ptr '@'             { VPUSH($$,$1,new PtrTypeNode(0)); }
        | type_ptr '[' INTEGER ']' { VPUSH($$,$1,new PtrTypeNode($3)); }
        |                          { $$ = VEMPTY; }
        ;

type_arg_list: '(' type_args ')' { $$ = $2; } | { $$ = VEMPTY; } ;
type_args: type_args ',' type_id { VPUSH($$,$1,$3); }
         | type_id               { VINIT($$,$1); }
         ;


// Auxiliares para lidar com identificadores
pro_id: PRO_ID { $$ = $1;} | QPRO_ID { $$ = $1;} ;
com_id: COM_ID { $$ = $1;} | QCOM_ID { $$ = $1;} ;
sym_id_: SYM_ID_R8 { $$ = $1; }
       | SYM_ID_L7 { $$ = $1; }
       | SYM_ID_L6 { $$ = $1; }
       | SYM_ID_L5 { $$ = $1; }
       | SYM_ID_R5 { $$ = $1; }
       | SYM_ID_N4 { $$ = $1; }
       | SYM_ID_L3 { $$ = $1; }
       | SYM_ID_L2 { $$ = $1; }
       | SYM_ID_L1 { $$ = $1; }
       | SYM_ID_R1 { $$ = $1; }
       | SYM_ID_N1 { $$ = $1; }
       | '-'       { $$ = new std::string("-"); }
       ;

// Auxiliar para uma literal qualquer
literal: INTEGER { $$ = new LiteralNode($1); }
       | REAL    { $$ = new LiteralNode($1); }
       | CHAR    { $$ = new LiteralNode($1); }
       | STRING  { $$ = new LiteralNode(*$1); }
       ;
%%

/* Código auxiliar */

void yyerror(const char *msg) {
    std::stringstream buffer;
    buffer << yylloc.first_line << ":" << yylloc.first_column << ": syntax unbalance: " << msg;
    errors.push_back(buffer.str());
}

void yyerror_(const char *msg, YYLTYPE loc) {
    std::stringstream buffer;
    buffer << loc.first_line << ":" << loc.first_column << ": syntax unbalance: " << msg;
    errors.push_back(buffer.str());
}

