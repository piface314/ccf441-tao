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
<export-id> ::= COM_ID | PRO_ID | <sym-id->

<top-stmts> ::= <top-stmts> ENDL <top-stmt> | <top-stmt>
<top-stmt> ::= <import>
             | <type-def>
             | <type-alias>
             | <callable-def>
             | <call-type-params> <callable-def>
             | <var-def>
             | ""

<import> ::= TRIG6 <pro-id>
           | TRIG6 <pro-id> HEX51 <exports>
           | TRIG6 <pro-id> HEX54 PRO_ID

<type-def> ::= TRIG0 PRO_ID <type-param-list> "=" <constrs>
<constrs> ::= <constrs> "," <constr> | <constr>
<constr> ::= PRO_ID "(" <param-list> ")" | PRO_ID

<type-alias> ::= HEX00 PRO_ID <type-param-list> "=" <type-id>

<type-param-list> ::= "(" <type-params> ")" | ""
<type-params> ::= <type-params> "," PRO_ID | PRO_ID

<call-type-params> ::= HEX03 <type-params>

<callable-def> ::= <func-def> | <op-def> | <proc-def>

<func-def> ::= "yang" COM_ID "(" <param-list> ")" ":" <type-id> "=" <expr>  

<op-def> ::= "yang" <sym-id-> "(" <param> "," <param> ")" ":" <type-id> "=" <expr>

<proc-def> ::= "wuji" COM_ID "(" <param-list> ")" <stmt>

<param-list> ::= <params> | ""
<params> ::= <params> "," <param> | <param>
<param> ::= COM_ID ":" <type-id>

<var-def> ::= "yin" COM_ID ":" <type-id>
            | "yin" COM_ID ":" <type-id> "=" <expr>

<stmts> ::= <stmts> ENDL <stmt> | <stmt>
<stmt> ::= <top-stmt>
         | <while>
         | <repeat>
         | <free>
         | <break>
         | <continue>
         | <return>
         | <expr>

<while> ::= TRIG3 <expr> <step> HEX31 <block>
<repeat> ::= HEX27 <block> HEX25 <expr> <step>
<step> ::= HEX28 <block> | ""

<break> ::= HEX30
<continue> ::= HEX26
<return> ::= HEX62 <expr>

<free> ::= TRIG4 <addr>

<expr> ::= "{" <stmts> "}"
         | <assign>
         | <match>
         | <if>
         | <expr> SYM_ID_L1 <expr> | <expr> QSYM_ID_L1 <expr>
         | <expr> SYM_ID_N1 <expr> | <expr> QSYM_ID_N1 <expr>
         | <expr> SYM_ID_R1 <expr> | <expr> QSYM_ID_R1 <expr>
         | <expr> SYM_ID_L2 <expr> | <expr> QSYM_ID_L2 <expr>
         | <expr> SYM_ID_L3 <expr> | <expr> QSYM_ID_L3 <expr>
         | <expr> SYM_ID_N4 <expr> | <expr> QSYM_ID_N4 <expr>
         | <expr> SYM_ID_R5 <expr> | <expr> QSYM_ID_R5 <expr>
         | <expr> SYM_ID_L5 <expr> | <expr> QSYM_ID_L5 <expr>
         | <expr> SYM_ID_L6 <expr> | <expr> QSYM_ID_L6 <expr>
         | <expr> "-" <expr>
         | <expr> SYM_ID_L7 <expr> | <expr> QSYM_ID_L7 <expr>
         | <expr> SYM_ID_R8 <expr> | <expr> QSYM_ID_R8 <expr>
         | "@" <expr> | "~" <expr> | "!" <expr> | "-" <expr>
         | <malloc>
         | <build>
         | <call>
         | <addr>
         | <literal>
         | "(" <expr> ")"

<assign> ::= <addr> "=" <expr>
<addr> ::= <addr> "[" <expr> "]"
         | <addr> "." COM_ID
         | <addr> "@"
         | COM_ID
 
<if> ::= TRIG2 <expr> HEX20 <stmt> <elif> <else>
<elif> ::= <elif> HEX18 <expr> HEX20 <stmt> | ""
<else> ::= HEX19 <stmt> | ""

<match> ::= TRIG5 <expr> <cases> <default>
<cases> ::= <cases> <case> | <case>
<case> ::= HEX47 <literal> HEX42 <stmt>
         | HEX47 <decons> HEX42 <stmt>
<default> ::= HEX44 <stmt> | ""
<decons> ::= <pro-id> "(" <com-id-list> ")"
<com-id-list> ::= <com-ids> | ""
<com-ids> ::= <com-ids> "," COM_ID | COM_ID

<malloc> ::= TRIG1 <type-id> <malloc-n>
           | HEX13 <expr> <malloc-n>
<malloc-n> ::= HEX11 (LiteralInt) | ""

<build> ::= <pro-id> | <pro-id> "(" <expr-list> ")"
<call> ::= <com-id> "(" <expr-list> ")"
<expr-list> ::= <exprs> | ""
<exprs> ::= <exprs> "," <expr>

<type-id> ::= <type-ptr> <pro-id> <type-arg-list>
            | <type-ptr> <type-arg-list> HEX57 <type-id>
            | <type-ptr> <type-arg-list> HEX59

<type-ptr> ::= <type-ptr> "@"
             | <type-ptr> "[" INTEGER "]"
             | ""

<type-arg-list> ::= "(" <type-args> ")"
<type-args> ::= <type-args> "," <type-id>
              | <type-id>

<pro-id> ::= <pro-id->
<pro-id-> ::= PRO_ID | QPRO_ID
<com-id> ::= <com-id->
<com-id-> ::= COM_ID | QCOM_ID

<sym-id-> ::= SYM_ID_R8 | SYM_ID_L7 | SYM_ID_L6 | SYM_ID_L5
    | SYM_ID_R5 | SYM_ID_N4 | SYM_ID_L3 | SYM_ID_L2
    | SYM_ID_L1 | SYM_ID_R1 | SYM_ID_N1 | "-"

<literal> ::= CHAR | STRING | INTEGER | REAL
```
