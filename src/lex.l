/* Análise Léxica */

%{
  #include "defs.h"
  #include "y.tab.h"
  #include <stdlib.h>
  #include <string.h>

  #define YY_USER_ACTION \
    yylloc.first_line = yylloc.last_line; \
    yylloc.first_column = yylloc.last_column; \
    if (!started) started = 1, printf("%3d: ", yylloc.last_line); \
    for (int i = 0; yytext[i]; i++) { \
      putchar(yytext[i]); \
      if (yytext[i] == '\n') \
        yylloc.last_line++, yylloc.last_column = 1, printf("%3d: ", yylloc.last_line); \
      else \
        yylloc.last_column++; \
    }

  #define STR_BUFFER_SIZE 0x1000 

  int started = 0;
  int id_token(int tk);
  int char_int_val(char c);
  int int_val(int base, int i0);
  double real_val();
  char unescape(char c);
  char *string_val();
  int tri_hex_val();
  void lexerror(char *msg);
  
  char buffer[STR_BUFFER_SIZE];
  char *s = NULL;
%}

%option yylineno

%x COMMENT
%x COMMENTS
%x STRLIT

/* Definições regulares */

ws            [ \n\t\r\v\f]
ws_space      [\n\t\r\v\f]
small         [a-z_]
large         [A-Z]
digit         [0-9]
hexit         [0-9A-Fa-f]
symbol_at     [!#$%&*+/<=>?^\|\\\-~:]
symbol        [!@#$%&*+/<=>?^\|\\\-~:]
special       [\(\),\[\]\{\}]
symid         {symbol_at}+|{symbol}*{symbol_at}{symbol}*
relop         ==|!=|\<|\<=|\>=|\>
comid         {small}({small}|{large}|{digit})*
proid         {large}({small}|{large}|{digit})*
qualify       ({proid}\.)+
decimal       {digit}+
exponent      [eE][+-]?{decimal}
real          ({decimal}\.{decimal}{exponent}?|{decimal}{exponent})
charesc       a|b|e|f|n|r|t|v|\\|\"|'
gap           \\{ws}+\\
trigram       [\|:]{3,3}
hexagram      [\|:]{6,6}

%%
{ws}+                       {/* ignore */}
;                           {return ENDL;}
''                          {BEGIN(COMMENT);}
<COMMENT>\n                 {BEGIN(INITIAL);}
<COMMENT>.                  {/* ignore */}
'''                         {BEGIN(COMMENTS);}
<COMMENTS>'''+              {BEGIN(INITIAL);}
<COMMENTS>\n                {/* ignore */}
<COMMENTS>.                 {/* ignore */}
wuji                        {return WUJI;}
yin                         {return YIN;}
yang                        {return YANG;}
{trigram}                   {return TRIG0 + tri_hex_val();}
{hexagram}                  {return HEX00 + tri_hex_val();}
[@:=\.\-]                   {return *yytext;}
{special}                   {return *yytext;}
{qualify}?\*\*              {return id_token(SYM_ID_R8);}
{qualify}?[*/&^]            {return id_token(SYM_ID_L7);}
{qualify}?[+\|]             {return id_token(SYM_ID_L6);}
{qualify}\-                 {return id_token(SYM_ID_L6);}
{qualify}?(\<\<|\>\>)       {return id_token(SYM_ID_L5);}
{qualify}?\<{symbol}*\>     {lexerror("invalid operator");}
{qualify}?\<\<{symbol}+     {return id_token(SYM_ID_L1);}
{qualify}?{symbol}+\>\>     {return id_token(SYM_ID_R1);}
{qualify}?\<{symbol}+       {return id_token(SYM_ID_L5);}
{qualify}?{symbol}+\>       {return id_token(SYM_ID_R5);}
{qualify}?{relop}           {return id_token(SYM_ID_N4);}
{qualify}?&&                {return id_token(SYM_ID_L3);}
{qualify}?\|\|              {return id_token(SYM_ID_L2);}
{qualify}?{symid}           {return id_token(SYM_ID_N1);}
{qualify}?{comid}           {return id_token(COM_ID);}
{qualify}?{proid}           {return id_token(PRO_ID);}
{decimal}                   {yylval.int_val = int_val(10, 0); return INTEGER;}
0[xX]{hexit}+               {yylval.int_val = int_val(16, 2); return INTEGER;}
{real}                      {yylval.real_val = real_val(); return REAL;}
'\\{charesc}'               {yylval.char_val = unescape(yytext[2]); return CHAR;}
'\\x{hexit}{hexit}'         {yylval.char_val = (char)((char_int_val(yytext[3]) << 4) + char_int_val(yytext[4])); return CHAR;}
'{ws_space}'                {lexerror("invalid char literal");}
'[^']'                      {yylval.char_val = yytext[1]; return CHAR;}
\"                          {BEGIN(STRLIT); s = buffer;}
<STRLIT>\"                  {BEGIN(INITIAL); *s = 0; yylval.string_val = string_val(); return STRING;}
<STRLIT>\\{charesc}         {*s++ = unescape(yytext[1]);}
<STRLIT>\\x{hexit}{hexit}   {*s++ = (char)((char_int_val(yytext[2]) << 4) + char_int_val(yytext[3]));}
<STRLIT>{gap}               {/* ignore */}
<STRLIT>{ws_space}          {lexerror("invalid string literal");}
<STRLIT>.                   {*s++ = *yytext;}
<STRLIT><<EOF>>             {lexerror("invalid string literal");}
.                           {lexerror("unknown symbol");}
%%

/* Código auxiliar */

int id_token(int tk) {
  yylval.string_val = strdup(yytext);
  for (int i = 0; yytext[i]; ++i)
    if (yytext[i] == '.')
      return tk+1;
  return tk;
}

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

double real_val() {
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

char unescape(char c) {
  switch (c) {
    case 'a': return '\a';
    case 'b': return '\b';
    case 'e': return '\e';
    case 'f': return '\f';
    case 'n': return '\n';
    case 'r': return '\r';
    case 't': return '\t';
    case 'v': return '\v';
    case '\\': return '\\';
    case '"': return '"';
    case '\'': return '\'';
    default: return -1;
  }
}

char *string_val() {
  return strdup(buffer);
}

int tri_hex_val() {
  int val = 0;
  for (int i = 0; yytext[i]; ++i)
    val = (val << 1) + (yytext[i]=='|');
  return val;
}

void lexerror(char *msg) {
  int line = yylloc.first_line;
  int col = yylloc.first_column;
  fprintf(stderr, "%d:%d: lexical unbalance: %s\n", line, col, msg);
  exit(1);
}

int yywrap(void) {
  return 1;
}
