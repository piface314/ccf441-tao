## Checklist
- [x] ```<program> ::= <module-decl> <top-stmts>```

- [x] ``` <module-decl> ::= TRIG7 <pro-id> HEX56 <exports> ENDL | "" ```
- [x] ``` <exports> ::= <exports> "," <export-id> | <export-id>```
- [x] ``` <export-id> ::= COM_ID | PRO_ID | SYM_ID```

- [x] ``` <top-stmts> ::= <top-stmts> ENDL <top-stmt> | <top-stmt>```
- [x] ```  <top-stmt> ::= <import>
             | <callable-def>
             | <def-type-params> <callable-def>
             | <var-def>
             | <type-def>
             | <type-alias>
             | "" ```

- [x] ``` <stmts> ::= <stmts> ENDL <stmt> | <stmt>```
- [ ] ``` <stmt> ::= <import>
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
         | "" ```

- [x] ``` <import> ::= TRIG6 <pro-id>
           | TRIG6 <pro-id> HEX51 <exports>
           | TRIG6 <pro-id> HEX54 PRO_ID ```

- [x] ``` <def-type-params> ::= HEX03 <type-params> ```
- [x] ``` <callable-def> ::= <func-def> | <op-def> | <proc-def> ```

- [ ] ``` <var-def> ::= "yin" COM_ID ":" <type-id> ```
            | "yin" COM_ID ":" <type-id> "=" <expr> ```

- [x] ``` <func-def> ::= "yang" COM_ID "(" <param-list> ")" ":" <type-id> "=" <expr>  ``` 

- [x] ``` <op-def> ::= "yang" <sym-id-> "(" <param> "," <param> ")" ":" <type-id> "=" <expr> ```

- [x] ``` <proc-def> ::= "wuji" COM_ID "(" <param-list> ")" <block> ```

- [x] ``` <type-def> ::= TRIG0 PRO_ID <type-param-list> "=" <constructors> ```
- [x] ``` <constructors> ::= <constructors> "," <constructor> | <constructor> ```
- [x] ``` <constructor> ::= PRO_ID "(" <param-list> ")" | PRO_ID; ```

- [x] ``` <type-alias> ::= HEX00 PRO_ID <type-param-list> "=" <type-id> ```

- [x] ``` <param-list> ::= <params> | "" ```
- [x] ``` <params> ::= <params> "," <param> | <param> ```
- [x] ``` <param> ::= COM_ID ":" <type-id> ```


- [ ] ``` <if> ::= TRIG2 <expr> HEX20 <stmt> <elif> <else> ```
- [ ] ``` <elif> ::= <elif> HEX18 <expr> HEX20 <expr> | "" ```
- [ ] ``` <else> ::= HEX19 <expr> | "" ```

- [ ] ``` <match> ::= TRIG5 <expr> <cases> <default> ```
- [ ] ``` <cases> ::= <cases> <case> | <case> ```
- [ ] ``` <case> ::= HEX47 <case-cond> HEX42 <expr> ```
- [ ] ``` <default> ::= HEX44 <expr> | "" ```
- [x] ``` <case-cond> ::= <literal> | <decons>  ```
- [x] ``` <decons> ::= <pro-id> "(" <com-id-list> ")" ```
- [x] ``` <com-id-list> ::= <com-ids> | "" ```
- [x] ``` <com-ids> ::= <com-ids> "," COM_ID | COM_ID ```

- [ ] ``` <while> ::= TRIG3 <expr> <step> HEX31 <block> ```
- [ ] ``` <repeat> ::= HEX27 <block> HEX25 <expr> <step> ```
- [x] ``` <step> ::= HEX28 <block> | "" ```

- [x] ``` <free> ::= TRIG4 <addr> ```

- [x] ``` <break> ::= HEX30 ```
- [x] ``` <continue> ::= HEX26 ```
- [ ] ``` <return> ::= HEX62 <expr> ```

- [ ] ``` <expr> ::= "{" <stmts> ENDL <expr> "}"
         | <if>
         | <match>
         | <assign>
         | <expr-addr> ```
- [x] ``` <expr-addr> ::= <expr-addr> "[" <expr> "]" ```
              | <expr-addr> "." COM_ID ```
              | <expr-unary> ```
- [x] ``` <expr-unary> ::= <unary-ops> <expr-1> | <expr-1> ```
- [x] ``` <unary-ops> ::= <unary-ops> <unary-op> ```
- [x] ``` <unary-op> ::= "@" | "$" | "~" | "!" | "-" ```

- [x] ``` <expr-9> ::= <literal>
           | <com-id>
           | <malloc>  
           | <build>
           | <call>
           | "(" <expr> ")" ```
           
- [x] ``` <assign> ::= <addr> "=" <expr> ```
- [x] ``` <addr> ::= <addr> "[" <expr> "]"
         | <addr> "." COM_ID
         | <addr-0> ```
- [x] ``` <addr-0> ::= <pointers> <addr-1>
- [x] ``` <addr-1> ::= <com-id> | "(" <addr> ")" ```

- [x] ``` <malloc> ::= <malloc-type> <type-id> <malloc-n>
           | <malloc-type> <expr> <malloc-n> ```
- [x] ``` <malloc-type> ::= TRIG1 | HEX12 ```
- [x] ``` <malloc-n> ::= HEX11 (LiteralInt) | "" ```

- [x] ``` <build> ::= <pro-id> | <pro-id> "(" <exprs> ")" ```
- [x] ``` <call> ::= <com-id> "(" <expr-list> ")" ```
- [x] ``` <expr-list> ::= <exprs> | "" ```
- [x] ``` <exprs> ::= <exprs> "," <expr> ```

- [x] ``` <block> ::= "{" <stmts> "}" | <stmt> ```

- [x] ``` <scope> ::= <scope> PRO_ID "." | "" ```
- [x] ``` <pro-id> ::= <scope> PRO_ID ```
- [x] ``` <com-id> ::= <scope> COM_ID ```

- [x] ``` <sym-id> ::= <scope> <sym-id-> ```
- [x] ``` <sym-id-> ::= SYM_ID_R8 | SYM_ID_L7 | SYM_ID_L6 | SYM_ID_L5
    | SYM_ID_R5 | SYM_ID_N4 | SYM_ID_L3 | SYM_ID_L2
    | SYM_ID_L1 | SYM_ID_R1 | SYM_ID_N1 ```

- [x] ``` <type-id> ::= <pointers> <pro-id> <type-param-list>
            | <pointers> HEX57 <type-param-list> HEX57 <type-id>
            | <pointers> HEX57 <type-param-list> ```
- [x] ``` <type-param-list> ::= "(" <type-params> ")" | "" ```
- [x] ``` <type-params> ::= <type-params> "," <type-id> | <type-id> ```
- [x] ``` <pointers> ::= <pointers> "@" | "" ```
- [x] ``` <literal> ::= CHAR | STRING | INTEGER | REAL ```
