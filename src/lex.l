/* Análise Léxica */

%{
  #include <string>
  #include <sstream>
  #include <iostream>
  #include <vector>
  #include "parser.hpp"
  #include "lexer.hpp"
  
  extern "C" int yywrap() {return 1;}

  #define YY_USER_ACTION lexer.update_yyloc();

  Lexer lexer;
%}

%option yylineno
%option nounput

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
symid         {symbol}*{symbol_at}{symbol}*
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
{trigram}                   {return lexer.trig_token();}
{hexagram}                  {return lexer.hex_token();}
[@:=\.\-]                   {return *yytext;}
{special}                   {return *yytext;}
{qualify}?\*\*              {return lexer.id_token(SYM_ID_R8);}
{qualify}?[*/&^]            {return lexer.id_token(SYM_ID_L7);}
{qualify}?[+\|]             {return lexer.id_token(SYM_ID_L6);}
{qualify}\-                 {return lexer.id_token(SYM_ID_L6);}
{qualify}?(\<\<|\>\>)       {return lexer.id_token(SYM_ID_L5);}
{qualify}?\<{symbol}*\>     {lexer.error("invalid operator");}
{qualify}?\<\<{symbol}+     {return lexer.id_token(SYM_ID_L1);}
{qualify}?{symbol}+\>\>     {return lexer.id_token(SYM_ID_R1);}
{qualify}?\<{symbol}+       {return lexer.id_token(SYM_ID_L5);}
{qualify}?{symbol}+\>       {return lexer.id_token(SYM_ID_R5);}
{qualify}?{relop}           {return lexer.id_token(SYM_ID_N4);}
{qualify}?&&                {return lexer.id_token(SYM_ID_L3);}
{qualify}?\|\|              {return lexer.id_token(SYM_ID_L2);}
{qualify}?{symid}           {return lexer.id_token(SYM_ID_N1);}
{qualify}?{comid}           {return lexer.id_token(COM_ID);}
{qualify}?{proid}           {return lexer.id_token(PRO_ID);}
{decimal}                   {return lexer.integer_token(10, 0);}
0[xX]{hexit}+               {return lexer.integer_token(16, 2);}
{real}                      {return lexer.real_token();}
'\\{charesc}'               {return lexer.char_esc_token();}
'\\x{hexit}{hexit}'         {return lexer.char_hex_token();}
'{ws_space}'                {lexer.error("invalid char literal");}
'[^']'                      {return lexer.char_token();}
\"                          {lexer.str_lit_begin();}
<STRLIT>\"                  {return lexer.string_token();}
<STRLIT>\\{charesc}         {lexer.str_lit_push_hex();}
<STRLIT>\\x{hexit}{hexit}   {lexer.str_lit_push_esc();}
<STRLIT>{gap}               {/* ignore */}
<STRLIT>{ws_space}          {lexer.error("invalid string literal");}
<STRLIT>.                   {lexer.str_lit_push();}
<STRLIT><<EOF>>             {lexer.error("invalid string literal");}
.                           {lexer.error("unknown symbol");}
%%

/* Código auxiliar */

void Lexer::update_yyloc() {
  yylloc.first_line = yylloc.last_line;
  yylloc.first_column = yylloc.last_column;
  for (int i = 0; yytext[i]; i++)
    if (yytext[i] == '\n')
      yylloc.last_line++, yylloc.last_column = 1;
    else
      yylloc.last_column++;
}

int Lexer::id_token(int tk) {
  yylval.string_val = new std::string(yytext, yyleng);
  for (int i = 0; yytext[i]; ++i)
    if (yytext[i] == '.')
      return tk+1;
  return tk;
}

int Lexer::char_int_val(char c) {
  if ('0' <= c && c <= '9')
    return c - '0';
  else if ('a' <= c && c <= 'z')
    return c - 'a' + 10;
  else if ('A' <= c && c <= 'Z')
    return c - 'A' + 10;
  else
    return 0;
}

int Lexer::int_val(int base, int i0) {
  int v = 0;
  for (int i = i0; i < yyleng; ++i)
    v = v * base + char_int_val(yytext[i]);
  return v;
}

double Lexer::real_val() {
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

char Lexer::unescape(char c) {
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

int Lexer::tri_hex_offset() {
  int offset = 0;
  for (int i = 0; yytext[i]; ++i)
    offset = (offset << 1) + (yytext[i]=='|');
  return offset;
}

int Lexer::trig_token() {
  return TRIG0 + tri_hex_offset();
}
int Lexer::hex_token() {
  return HEX00 + tri_hex_offset();
}

int Lexer::integer_token(int base, int i0) {
  yylval.int_val = int_val(base, i0);
  return INTEGER;
}

int Lexer::real_token() {
  yylval.real_val = real_val();
  return REAL;
}

int Lexer::char_token() {
  yylval.char_val = yytext[1];
  return CHAR;
}

int Lexer::char_esc_token() {
  yylval.char_val = unescape(yytext[2]);
  return CHAR;
}

int Lexer::char_hex_token() {
  yylval.char_val = (char)(
    (char_int_val(yytext[3]) << 4)
    + char_int_val(yytext[4])
  );
  return CHAR;
}

void Lexer::str_lit_begin() {
  BEGIN(STRLIT);
  buffer = std::stringstream();
}

int Lexer::string_token() {
  BEGIN(INITIAL);
  yylval.string_val = new std::string(buffer.str());
  return STRING;
}

void Lexer::str_lit_push() {
  buffer << *yytext;
}

void Lexer::str_lit_push_esc() {
  buffer << unescape(yytext[1]);
}

void Lexer::str_lit_push_hex() {
  buffer << (char)((char_int_val(yytext[2]) << 4) + char_int_val(yytext[3]));
}

void Lexer::error(std::string msg) {
  int line = yylloc.first_line;
  int col = yylloc.first_column;
  std::cerr << line << ':' << col << ": lexical unbalance: " << msg << std::endl;
  exit(1);
}

  /* 
  int yywrap(void) {
    return 1;
  } */

