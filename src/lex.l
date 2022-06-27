/* Análise Léxica */

%{
#define SHOW(TOKEN)    printf("(%s)\n", TOKEN)
#define SHOW_T(TOKEN)  printf("(%s, `%s`)\n", TOKEN, yytext)
#define SHOW_I(TOKEN, V)  printf("(%s, %d)\n", TOKEN, V)
#define SHOW_F(TOKEN, V)  printf("(%s, %lf)\n", TOKEN, V)

int char_int_val(char c);
int int_val(int base, int i0);
double float_val();
%}
%option noyywrap

/* Definições regulares */

ws            [ \n\t\r\v\f]
small         [a-z_]
large         [A-Z]
digit         [0-9]
any_sym       {small}|{large}|{digit}|["' \t]
any           {any_sym}|{symbol}
symbol_cb     [!#$%&*+\./<=>?@^\|\-~]
symbol_b      {symbol_cb}|:
symbol_c      {symbol_cb}|\\
symbol        {symbol_c}|:
comment       \'\'+({any_sym}{any}*)?\n
comments      \'\'\'([^\'''])*\'\'\'
special       [\(\),;\[\]`\{\}]
SymId         {any_sym}
ComId         {small}({small}|{large}|{digit}|')*
ProId         {large}({small}|{large}|{digit}|')*
exponent      [eE][+-]?{decimal}
float         ({decimal}\.{decimal}{exponent}?|{decimal}{exponent})
decimal       {digit}+
charesc       a|b|e|f|n|r|t|v|\\|\"|'
cntrl         @|\[|\\|\]|ˆ|_
ascii         \^{cntrl}|NUL|SOH|STX|ETX|EOT|ENQ|ACK|BEL|BS|HT|LF|VT|FF|CR|SO|SI|DLE|DC1|DC2|DC3|DC4|NAK|SYN|ETB|CAN|EM|SUB|ESC|FS|GS|RS|US|SP|DEL
escape        \\({charesc}|{ascii}|{decimal})
gap           \\{ws}+\\
char          '({small}|{large}|{symbol_b}|{digit}|{special}|\"|" "|{escape})'
string        \"({small}|{large}|{symbol_b}|{digit}|{special}|\'|" "|{escape}|{gap})*\"



%%
({ws}|{comment}|{comments})+    {/* ignore */}
yin                             {SHOW("Yin");}
yang                            {SHOW("Yang");}
wuji                            {SHOW("Wuji");}
\|\|\|                          {SHOW("SymExportModule");}
\|\|\:                          {SHOW("SymImportModule");}
\|\|\:\|\|                      {SHOW("SymFromImport");}
\|\|\:\|\|\:                    {SHOW("SymImportAs");}
\|\|\|\:\:\:                    {SHOW("SymExportMethods");}
\|\:\|                          {SHOW("SymMatch");}
\|\:\|\|\|\|                    {SHOW("SymMatchCase");}
\|\:\|\:\|\:                    {SHOW("SymBlock");}
\:\|\:                          {SHOW("SymIf");}
\:\|\:\|\:\:                    {SHOW("SymThen");}
\:\|\:\:\|\:                    {SHOW("SymElif");}
\:\|\:\:\|\|                    {SHOW("SymElse");}
\|\:\:                          {SHOW("SymFree");}
\:\|\|                          {SHOW("SymLoop");}
\:\:\|                          {SHOW("SymMemAlloc");}
\:\:\:                          {SHOW("SymType");}
\>                              {SHOW("SymGT");}
\<                              {SHOW("SymLT");}
\<\=                            {SHOW("SymLE");}
\>\=                            {SHOW("SymGE");}
\=                              {SHOW("SymEQ");}
\!\=                            {SHOW("SymNE");}
@                               {SHOW("SymPointer");}
{special}                       {SHOW_T("Special");}
{SymId}                         {SHOW_T("SymId");}
{ComId}                         {SHOW_T("ComId");}
{ProId}                         {SHOW_T("ProId");}
{decimal}                       {SHOW_I("LiteralInt", int_val(10, 0));}
{float}                         {SHOW_F("LiteralFloat", float_val());}
{char}                          {SHOW_T("LiteralChar");}
{string}                        {SHOW_T("LiteralString");}


%%
int char_int_val(char c) {
  if ('0' <= c && c <= '9')
    return c - '0';
  else if ('a' <= c && c <= 'z')
    return c - 'a' + 10;
  else if ('A' <= c && c <= 'Z')
    return c - 'A' + 10;
  else
    return 0;
}

int int_val(int base, int i0) {
  int v = 0;
  for (int i = i0; i < yyleng; ++i)
    v = v * base + char_int_val(yytext[i]);
  return v;
}

double float_val() {
  double v = 0;
  int i = 0;
  for (; '0' <= yytext[i] && yytext[i] <= '9'; ++i)
    v = v * 10.0 + yytext[i] - '0';
  if (yytext[i++] == '.') {
    double e = 0.1;
    for (; '0' <= yytext[i] && yytext[i] <= '9'; ++i) {
      v = v + (yytext[i] - '0') * e;
      e /= 10;
    }
  }
  if (i++ < yyleng) {
    int sign = 1;
    int e = 0;
    if (yytext[i] == '-') 
      sign = -1, ++i;
    else if (yytext[i] == '+')
      ++i;
    for (; i < yyleng; ++i)
      e = e * 10 + yytext[i] - '0';
    if (sign > 0)
      for (int k = 0; k < e; ++k)
        v *= 10;
    else
      for (int k = 0; k < e; ++k)
        v /= 10;
  }
  return v;
}

/* Código auxiliar */
int main() {
  yylex();
  return 0;
}