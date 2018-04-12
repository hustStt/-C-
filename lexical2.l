%{
     #include "syntax.tab.h"
     #include "tmain.h"
%}
%option yylineno
digit [0-9]
number {digit}+
letter [a-zA-Z]
INT  ([1-9][0-9]*)|0|(0[0-7]+)|(0[Xx][0-9A-Fa-f]+)
FLOAT  ([0-9]*\.?[0-9]+|[0-9]+\.)([eE][+-]?[0-9]+)?
SEMI [;]
COMMA [,]
ASSIGNOP [=]
PLUS [+]
MINUS [-]
STAR [*]
DIV [/]
AND [&][&]
OR [|][|]
DOT [.]
NOT [!]
LP \(
RP \)
LB \[
RB \]
LC \{
RC \}
ID  (_|{letter})({letter}|{digit}|_)*
RELOP >|<|>=|<=|==|!=
TYPE  int|float
STRUCT struct
RETURN return
IF if
ELSE else
WHILE while
blank [\t\x0B\f\r\x20]+
zhushi \/\/.*
zhushi2 \/\*.*\n*.*\*\/
%%
{blank}   {}
"\n"   {}
{zhushi}  {}
{zhushi2}  {}
{INT}  {/*printf("(INT,%s)\n",yytext);*/yylval.a = newast("INT",0,yylineno);return INT;}
{FLOAT}  {/*printf("(FLOAT,%s)\n",yytext);*/yylval.a = newast("FLOAT",0,yylineno);return FLOAT;}
{SEMI}  {/*printf("(SEMI,%s)\n",yytext);*/yylval.a = newast("SEMI",0,yylineno);return SEMI;}
{COMMA}  {/*printf("(COMMA,%s)\n",yytext);*/yylval.a = newast("COMMA",0,yylineno);return COMMA;}
{ASSIGNOP}  {/*printf("(ASSIGNOP,%s)\n",yytext);*/yylval.a = newast("ASSIGNOP",0,yylineno);return ASSIGNOP;}
{RELOP}  {/*printf("(RELOP,%s)\n",yytext);*/yylval.a = newast("RELOP",0,yylineno);return RELOP;}
{PLUS}  {/*printf("(PLUS,%s)\n",yytext);*/yylval.a = newast("PLUS",0,yylineno);return PLUS;}
{MINUS}  {/*printf("(MINUS,%s)\n",yytext);*/yylval.a = newast("MINUS",0,yylineno);return MINUS;}
{STAR}  {/*printf("(STAR,%s)\n",yytext);*/yylval.a = newast("STAR",0,yylineno);return STAR;}
{DIV}  {/*printf("(DIV,%s)\n",yytext);*/yylval.a = newast("DIV",0,yylineno);return DIV;}
{AND}  {/*printf("(AND,%s)\n",yytext);*/yylval.a = newast("AND",0,yylineno);return AND;}
{OR}  {/*printf("(OR,%s)\n",yytext);*/yylval.a = newast("OR",0,yylineno);return OR;}
{DOT}  {/*printf("(DOT,%s)\n",yytext);*/yylval.a = newast("DOT",0,yylineno);return DOT;}
{NOT}  {/*printf("(NOT,%s)\n",yytext);*/yylval.a = newast("NOT",0,yylineno);return NOT;}
{TYPE}  {/*printf("(TYPE,%s)\n",yytext);*/yylval.a = newast("TYPE",0,yylineno);return TYPE;}
{LP}  {/*printf("(LP,%s)\n",yytext);*/yylval.a = newast("LP",0,yylineno);return LP;}
{RP}  {/*printf("(RP,%s)\n",yytext);*/yylval.a = newast("RP",0,yylineno);return RP;}
{LB}  {/*printf("(LB,%s)\n",yytext);*/yylval.a = newast("LB",0,yylineno);return LB;}
{RB}  {/*printf("(RB,%s)\n",yytext);*/yylval.a = newast("RB",0,yylineno);return RB;}
{LC}  {/*printf("(LC,%s)\n",yytext);*/yylval.a = newast("LC",0,yylineno);return LC;}
{RC}  {/*printf("(RC,%s)\n",yytext);*/yylval.a = newast("RC",0,yylineno);return RC;}
{STRUCT}  {/*printf("(STRUCT,%s)\n",yytext);*/yylval.a = newast("STRUCT",0,yylineno);return STRUCT;}
{RETURN}  {/*printf("(RETURN,%s)\n",yytext);*/yylval.a = newast("RETURN",0,yylineno);return RETURN;}
{IF}  {/*printf("(IF,%s)\n",yytext);*/yylval.a = newast("IF",0,yylineno);return IF;}
{ELSE}  {/*printf("(ELSE,%s)\n",yytext);*/yylval.a = newast("ELSE",0,yylineno);return ELSE;}
{WHILE}  {/*printf("(WHILE,%s)\n",yytext);*/yylval.a = newast("WHILE",0,yylineno);return WHILE;}
{ID}  {/*printf("(ID,%s)\n",yytext);*/yylval.a = newast("ID",0,yylineno);return ID;}
.   {
	printf("Error type A at Line %d: Mysterious characters \'%s\'\n",yylineno,yytext);
    }
%%
