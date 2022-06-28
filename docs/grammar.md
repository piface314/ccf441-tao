# Gramática da Linguagem

Podemos definir a gramática usando [BNF](https://en.wikipedia.org/wiki/Backus%E2%80%93Naur_form), pra padronizar a notação.


## Legenda
```
<variavel-da-gramatica>
{NomeDeToken}
"literal"

"" : palavra vazia
{ComID}: identificador comum (começa com minúscula) `index`, `x`, `main`
{ProID}: identificador próprio (começa com maiúscula) `Tree`, `Node`, `List`
{SymID}: identificador de símbolo `+`, `-`, `*`, `/`
{QComID}: identificador comum qualificado `Tree.add`, `List.remove`
{QProID}: identificador próprio qualificado `Set.Empty`, `Model.Base.SQL`
{QSymID}: identificador de símbolo qualificado `Tree.+>`, `List.<<`
{TrigX}: trigrama de número X `Trig6 = ||:` `Trig5 = |:|` `Trig0 = :::`
{HexXX}: hexagrama de número XX `Hex00 = ::::::` `Hex42 = |:|:|:`
{Endl}: Fim de linha (pode ser \n ou ;)
```

## Definição
```
<program> ::= <module-decl> <top-stmts>

<module-decl> ::= {Trig7} <pro-id> {Hex56} <exports> {Endl} | ""
<exports> ::= <export> "," <export-id> | <export-id>
<export-id> ::= {ComID} | {ProID} | {SymID}

<top-stmts> ::= <top-stmts> {Endl} <top-stmt> | <top-stmt>
<top-stmt> ::= <import>
             | <definition>
             | <def-type-params> <definition>
             | ""

<import> ::= {Trig6} <pro-id>
           | {Trig6} <pro-id> {Hex51} <export-list>
           | {Trig6} <pro-id> {Hex54} {ProID} 

<def-type-params> ::= {Hex03} <type-params>
<definition> ::= <var-def>
               | <func-def>
               | <op-def>
               | <proc-def>
               | <type-def>

<var-def> ::= "yin" {ComID} ":" <type-id> "=" <expr>
<func-def> ::= "yang" {ComID} "(" <param-list> ")" ":" <type-id> "=" <expr>  
<op-def> ::= "yang" <op-assoc> <op-prec> {SymID} "(" <param> "," <param> ")" ":" <type-id> "=" <expr>
<op-assoc> ::= {Hex04} | {Hex05} | {Hex06}
<op-prec> ::= "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8"
<proc-def> ::= "wuji" {ComID} "(" <param-list> ")" <block>
<type-def> ::= {Trig0} {ProID} <type-param-list> "=" <constructors>
<constructors> ::= <constructors> "," <constructor> | <constructor>
<constructor> ::= {ProID} "(" <param-list> ")"
<param-list> ::= <params> | ""
<params> ::= <params> "," <param> | <param>
<param> ::= {ComID} ":" <type-id>

<stmts> ::= <stmts> {Endl} <stmt> | <stmt>
<stmt> ::= <import>
         | <definition>
         | <def-type-params> <definition>
         | <call>
         | <assign-stmt>
         | <if-stmt>
         | <match>
         | <while>
         | <repeat>
         | <free>
         | ""

<assign-stmt> ::= <addr> "=" <expr>
<addr> ::= <addr> "[" <expr> "]"
         | <addr> "." {ComID}
         | <addr-0>
<addr-0> ::= <pointers> <addr-1>
<addr-1> ::= <com-id> | "(" <addr> ")"

<if-stmt> ::= {Trig2} <expr> {Hex20} <expr> <elif> <else-stmt>
<if-expr> ::= {Trig2} <expr> {Hex20} <expr> <elif> <else>
<else-stmt> ::= <else> | ""
<else> ::= {Hex19} <expr>
<elif> ::= <elif> {Hex18} <expr> {Hex20} <expr> | ""

<match> ::= {Trig5} <expr> <cases> <default>
<cases> ::= <cases> <case> | <case>
<case> ::= {Hex47} <case-cond> {Hex42} <expr>
<default> ::= {Hex44} <expr> | ""
<case-cond> ::= <literal> | <decons> 
<decons> ::= <pro-id> "(" <com-id-list> ")"
<com-id-list> ::= <com-ids> | ""
<com-ids> ::= <com-ids> "," {ComID} | {ComID}

<while> ::= {Trig3} <expr> <step> {Hex31} <expr>
<repeat> ::= {Hex27} <expr> {Hex25} <expr> <step>
<step> ::= {Hex28} <block> | ""

<free> ::= {Trig4} <addr>
<malloc> ::= {Trig1} <type-id> <malloc-n>
           | {Hex12} <type-id> <malloc-n>
<malloc-n> ::= {Hex11} {IntLiteral} | ""

<expr> ::= "{" <stmts> {Endl} <expr> "}" | <expr-addr>
<expr-addr> ::= <expr-addr> "[" <expr> "]"
              | <expr-addr> "." {ComID}
              | <expr-unary>
<expr-unary> ::= <unary-ops> <expr-1> | <expr-1>
<unary-ops> ::= <unary-ops> <unary-op>
<unary-op> ::= "@" | "$" | "~" | "-"
<expr-N> ::= <expr-l-N> | <expr-r-N> 
<expr-l-N> ::= <expr-l-N> {OpLN} <expr-l-N+1>
<expr-r-N> ::= <expr-r-N+1> {OpRN} <expr-r-N>
<expr-9> ::= <literal>
           | <com-id>
           | <malloc>
           | <if-expr>
           | <match>
           | <build>
           | <call>
           | "(" <expr> ")"


<build> ::= <pro-id> "(" <expr-list> ")"
<call> ::= <com-id> "(" <expr-list> ")"
<expr-list> ::= <exprs> | ""
<exprs> ::= <exprs> "," <expr>

<block> ::= "{" <stmts> "}" | <stmt>

<pro-id> ::= {ProID} | {QProID}
<com-id> ::= {ComID} | {QComID}

<type-id> ::= <pointers> {QProID} <type-param-list>
<type-param-list> ::= "(" <type-params> ")" | ""
<type-params> ::= <type-params> "," <type-id> | <type-id>
<pointers> ::= <pointers> "@" | ""

```
