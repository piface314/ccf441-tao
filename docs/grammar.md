# Gramática da Linguagem

Podemos definir a gramática usando [BNF](https://en.wikipedia.org/wiki/Backus%E2%80%93Naur_form), pra padronizar a notação.
Por exemplo:

```
<expr> ::= <expr> "+" <term> | <term>
<term> ::= <term> "*" <fact> | <fact>
<fact> ::= "(" <expr> ")" | "id"
```