/* Análise Léxica */

%{
#define SHOW(TOKEN)       printf("%d@(%s)\n", line, TOKEN)
#define SHOW_T(TOKEN)     printf("%d@(%s, `%s`)\n", line, TOKEN, yytext)
#define SHOW_I(TOKEN, V)  printf("%d@(%s, %d)\n", line, TOKEN, V)
#define SHOW_F(TOKEN, V)  printf("%d@(%s, %lf)\n", line, TOKEN, V)

int char_int_val(char c);
int int_val(int base, int i0);
double real_val();
int tri_hex_val();
int line = 1;
%}

%option noyywrap
%x COMMENT
%x COMMENTS

/* Definições regulares */

ws            [ \n\t\r\v\f]
ws_newline    [ \t\r\v\f]
small         [a-z_]
large         [A-Z]
digit         [0-9]
hexit         [0-9A-Fa-f]
symbol        [!#$%&*+\./<=>?@^\|\-~:]
special       [\(\),\[\]\{\}]
symid         {symbol}+
comid         {small}({small}|{large}|{digit})*
proid         {large}({small}|{large}|{digit})*
qualify       ({proid}\.)+
decimal       {digit}+
exponent      [eE][+-]?{decimal}
real          ({decimal}\.{decimal}{exponent}?|{decimal}{exponent})
charesc       a|b|e|f|n|r|t|v|\\|\"|'
escape        \\({charesc}|x{hexit}{hexit})
gap           \\{ws}+\\
char          '([^'\\]|{escape})'
string        \"([^"\\]|{escape}|{gap})*\"
trigram       [\|:]{3,3}
hexagram      [\|:]{6,6}



%%
\n                              {SHOW("Endl"); line++;}
;                               {SHOW("Endl");}
''                              {BEGIN(COMMENT);}
<COMMENT>\n                     {BEGIN(INITIAL);line++;}
<COMMENT>.                      {/* ignore */}
'''                             {BEGIN(COMMENTS);}
<COMMENTS>\n                    {line++;}
<COMMENTS>'''+                  {BEGIN(INITIAL);}
<COMMENTS>.                     {/* ignore */}
{ws_newline}+                   {/* ignore */}
yin                             {SHOW("Yin");}
yang                            {SHOW("Yang");}
wuji                            {SHOW("Wuji");}
{trigram}                       {SHOW_I("Trig", tri_hex_val());}
{hexagram}                      {SHOW_I("Hex", tri_hex_val());}
@                               {SHOW("SymPointer");}
:                               {SHOW("SymTypeTag");}
=                               {SHOW("SymAssign");}
\.                              {SHOW("SymAccess");}
{special}                       {SHOW_T("Special");}
{symid}                         {SHOW_T("SymId");}
{comid}                         {SHOW_T("ComId");}
{proid}                         {SHOW_T("ProId");}
{qualify}{symid}                {SHOW_T("QSymId");}
{qualify}{comid}                {SHOW_T("QComId");}
{qualify}{proid}                {SHOW_T("QProId");}
{decimal}                       {SHOW_I("LiteralInt", int_val(10, 0));}
0[xX]{hexit}+                   {SHOW_I("LiteralInt", int_val(16, 2));}
{real}                          {SHOW_F("LiteralReal", real_val());}
{char}                          {SHOW_T("LiteralChar");}
{string}                        {SHOW_T("LiteralString");}
.                               {printf("Lexical unbalance at line %d: unknown symbol `%s`\n", line, yytext);}

%%

/* Código auxiliar */

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

int tri_hex_val() {
  int val = 0;
  for (int i = 0; yytext[i]; ++i)
    val = (val << 1) + (yytext[i]=='|');
  return val;
}

int main() {
  yylex();
  return 0;
}
