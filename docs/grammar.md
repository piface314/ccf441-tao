# Gramática da Linguagem

Podemos definir a gramática usando [BNF](https://en.wikipedia.org/wiki/Backus%E2%80%93Naur_form), pra padronizar a notação.

## Operadores

Os operadores contendo `?` indicam que podem ser definidos pelo usuário.

| Precedência | Operadores        | Associatividade | Nome do Token |
|:------------|:------------------|:----------------|:--------------|
| 8           | `**`              | Direita         | `SYM_ID_R8`   |
| 7           | `* / & ^`         | Esquerda        | `SYM_ID_L7`   |
| 6           | `+ - \|`          | Esquerda        | `SYM_ID_L6`   |
| 5           | `<< >>`           | Esquerda        | `SYM_ID_L5`   |
| 5           | `<?`              | Esquerda        | `SYM_ID_L5`   |
| 5           | `?>`              | Direita         | `SYM_ID_R5`   |
| 4           | `== != < <= >= >` | Não associativo | `SYM_ID_N4`   |
| 3           | `&&`              | Esquerda        | `SYM_ID_L3`   |
| 2           | `\|\|`            | Esquerda        | `SYM_ID_L2`   |
| 1           | `<<?`             | Esquerda        | `SYM_ID_L1`   |
| 1           | `?>>`             | Direita         | `SYM_ID_R1`   |
| 1           | `?`               | Não associativo | `SYM_ID_N1`   |


## Legenda
```
<variavel-da-gramatica>
NOME_DE_TOKEN
"literal"

"" : palavra vazia
COM_ID: identificador comum (começa com minúscula) `index`, `x`, `main`
PRO_ID: identificador próprio (começa com maiúscula) `Tree`, `Node`, `List`
SYM_ID: identificador de símbolo `+`, `-`, `*`, `/`
QCOM_ID: identificador comum qualificado `Tree.add`, `List.remove`
QPRO_ID: identificador próprio qualificado `Set.Empty`, `Model.Base.SQL`
QSYM_ID: identificador de símbolo qualificado `Tree.+>`, `List.<<`
TRIGX: trigrama de número X `Trig6 = ||:` `Trig5 = |:|` `Trig0 = :::`
HEXXX: hexagrama de número XX `Hex00 = ::::::` `Hex42 = |:|:|:`
ENDL: Fim de linha (pode ser \n ou ;)
```

## Definição
```
<program> ::= <module-decl> <top-stmts>

<module-decl> ::= TRIG7 <pro-id> HEX56 <exports> ENDL | ""
<exports> ::= <exports> "," <export-id> | <export-id>
<export-id> ::= COM_ID | PRO_ID | SYM_ID

<top-stmts> ::= <top-stmts> ENDL <top-stmt> | <top-stmt>
<top-stmt> ::= <import>
             | <callable-def>
             | <def-type-params> <callable-def>
             | <var-def>
             | <type-def>
             | <type-alias>
             | ""

<stmts> ::= <stmts> ENDL <stmt> | <stmt>
<stmt> ::= <import>
         | <callable-def>
         | <def-type-params> <callable-def>
         | <var-def>
         | <type-def>
         | <type-alias>
         | <call>
         | <if-stmt>
         | <match>
         | <while>
         | <repeat>
         | <free>
         | <break>
         | <continue>
         | <return>
         | <expr>
         | ""

<import> ::= TRIG6 <pro-id>
           | TRIG6 <pro-id> HEX51 <exports>
           | TRIG6 <pro-id> HEX54 PRO_ID 

<def-type-params> ::= HEX03 <type-params>
<callable-def> ::= <func-def> | <op-def> | <proc-def>

<var-def> ::= "yin" COM_ID ":" <type-id>
            | "yin" COM_ID ":" <type-id> "=" <expr>

<func-def> ::= "yang" COM_ID "(" <param-list> ")" ":" <type-id> "=" <expr>  

<op-def> ::= "yang" <sym-id-> "(" <param> "," <param> ")" ":" <type-id> "=" <expr>

<proc-def> ::= "wuji" COM_ID "(" <param-list> ")" <block>

<type-def> ::= TRIG0 PRO_ID <type-param-list> "=" <constructors>
<constructors> ::= <constructors> "," <constructor> | <constructor>
<constructor> ::= PRO_ID "(" <param-list> ")" | PRO_ID;

<type-alias> ::= HEX00 PRO_ID <type-param-list> "=" <type-id>

<param-list> ::= <params> | ""
<params> ::= <params> "," <param> | <param>
<param> ::= COM_ID ":" <type-id>


<if> ::= TRIG2 <expr> HEX20 <stmt> <elif> <else>
<elif> ::= <elif> HEX18 <expr> HEX20 <expr> | ""
<else> ::= HEX19 <expr> | ""

<match> ::= TRIG5 <expr> <cases> <default>
<cases> ::= <cases> <case> | <case>
<case> ::= HEX47 <case-cond> HEX42 <expr>
<default> ::= HEX44 <expr> | ""
<case-cond> ::= <literal> | <decons> 
<decons> ::= <pro-id> "(" <com-id-list> ")"
<com-id-list> ::= <com-ids> | ""
<com-ids> ::= <com-ids> "," COM_ID | COM_ID

<while> ::= TRIG3 <expr> <step> HEX31 <block>
<repeat> ::= HEX27 <block> HEX25 <expr> <step>
<step> ::= HEX28 <block> | ""

<free> ::= TRIG4 <addr>

<break> ::= HEX30
<continue> ::= HEX26
<return> ::= HEX62 <expr>

<expr> ::= "{" <stmts> ENDL <expr> "}"
         | <if>
         | <match>
         | <assign>
         | <expr-addr>
<expr-addr> ::= <expr-addr> "[" <expr> "]"
              | <expr-addr> "." COM_ID
              | <expr-unary>
<expr-unary> ::= <unary-ops> <expr-1> | <expr-1>
<unary-ops> ::= <unary-ops> <unary-op>
<unary-op> ::= "@" | "$" | "~" | "!" | "-"

<expr> ::= <expr> SYM_ID_L5 <expr>
<expr_1>: <expr_1> SYM_ID_L1 <expr_1> 
    | <expr_1> SYM_ID_N1 <expr_1> 
    | <expr_1> SYM_ID_R1 <expr_1>
    | <expr_1> SYM_ID_L2 <expr_1>
    | <expr_1> SYM_ID_L3 <expr_1>
    | <expr_1> SYM_ID_N4 <expr_1>
    | <expr_1> SYM_ID_R5 <expr_1>
    | <expr_1> SYM_ID_L5 <expr_1>
    | <expr_1> SYM_ID_L6 <expr_1>
    | <expr_1> SYM_ID_L7 <expr_1>
    | <expr_1> SYM_ID_R8 <expr_1>
    
<expr-9> ::= <literal>
           | <com-id>
           | <malloc>  
           | <build>
           | <call>
           | "(" <expr> ")"
           
<assign> ::= <addr> "=" <expr>
<addr> ::= <addr> "[" <expr> "]"
         | <addr> "." COM_ID
         | <addr-0>
<addr-0> ::= <pointers> <addr-1>
<addr-1> ::= <com-id> | "(" <addr> ")"

<malloc> ::= <malloc-type> <type-id> <malloc-n>
           | <malloc-type> <expr> <malloc-n>
<malloc-type> ::= TRIG1 | HEX12
<malloc-n> ::= HEX11 (LiteralInt) | ""

<build> ::= <pro-id> | <pro-id> "(" <exprs> ")"
<call> ::= <com-id> "(" <expr-list> ")"
<expr-list> ::= <exprs> | ""
<exprs> ::= <exprs> "," <expr>

<block> ::= "{" <stmts> "}" | <stmt>

<scope> ::= <scope> PRO_ID "." | ""
<pro-id> ::= <scope> PRO_ID
<com-id> ::= <scope> COM_ID

<sym-id> ::= <scope> <sym-id->
<sym-id-> ::= SYM_ID_R8 | SYM_ID_L7 | SYM_ID_L6 | SYM_ID_L5
    | SYM_ID_R5 | SYM_ID_N4 | SYM_ID_L3 | SYM_ID_L2
    | SYM_ID_L1 | SYM_ID_R1 | SYM_ID_N1

<type-id> ::= <pointers> <pro-id> <type-param-list>
            | <pointers> HEX57 <type-param-list> HEX57 <type-id>
            | <pointers> HEX57 <type-param-list>
<type-param-list> ::= "(" <type-params> ")" | ""
<type-params> ::= <type-params> "," <type-id> | <type-id>
<pointers> ::= <pointers> "@" | ""
<literal> ::= CHAR | STRING | INTEGER | REAL
```
