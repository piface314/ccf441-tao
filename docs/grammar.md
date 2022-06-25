# Gramática da Linguagem

Podemos definir a gramática usando [BNF](https://en.wikipedia.org/wiki/Backus%E2%80%93Naur_form), pra padronizar a notação.


## Legenda
```
<variavel-da-gramatica>
{NomeDeToken}
"literal"

"" : palavra vazia
{ComID}: identificador comum (começa com minúscula)
{ProID}: identificador próprio (começa com maiúscula)
{SymID}: identificador de símbolo
{QComID}: identificador comum qualificado (tem o prefixo de módulo)
{QProID}: identificador próprio qualificado
{QSymID}: identificador de símbolo qualificado
{TrigX}: trigrama de número X
{HexXX}: hexagrama de número XX
{Endl}: Fim de linha (pode ser \n ou ;)
```

## Definição
```
<program> ::= <module-decl> <stmt-list>
<module-decl> ::= {Trig7} <pro-id> {Hex56} <export-list> {Endl} | ""
<export-list> ::= <export-list> "," <id> | <id>
<stmt-list> ::= <stmt-list> {Endl} <stmt> | <stmt>
<stmt> ::= <import>
         | <var-def>
         | <func-def>
         | <op-def>
         | <proc-def>
<import> ::= {Trig6} <pro-id>
           | {Trig6} <pro-id> {Hex51} <export-list>
           | {Trig6} <pro-id> {Hex54} {ProID}
<var-def> ::= "yin" {ComID} ":" <type-id> "=" <expr>
<func-def> ::= "yang" {ComID} "(" <param-list> ")" ":" <type-id> "=" <expr>  
<op-def> ::= "yang" {SymID} "(" <param> "," <param> ")" ":" <type-id> "=" <expr>
<proc-def> ::= "wuji" {ComID} "(" <param-list> ")" <block>
<param-list> ::= <params> | ""
<params> ::= <params> "," <param> | <param>
<param> ::= {ComID} ":" <type-id>  


<pro-id> ::= {ProID} | {QProID}
<id> ::= {ComID} | {ProID} | {SymID}
<type-id> ::= <pointers> {QProID}
            | <pointers> {QProID} "(" <type-params> ")"
<type-params> ::= <type-params> "," <type-id> | <type-id>
<pointers> ::= <pointers> "@" | ""

```
