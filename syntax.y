%{
  #include "table.h"
  # include<stdio.h>
  # define YYERROR_VERBOSE 1
  int Flag = 1;
%}

%union{
struct tree *a;
double d;
}

%token <a> INT FLOAT ID SEMI COMMA
%token <a> ASSIGNOP RELOP PLUS MINUS
%token <a> STAR DIV AND OR
%token <a> DOT NOT TYPE LP
%token <a> RP LB RB LC
%token <a> RC STRUCT RETURN IF ELSE WHILE

%type  <a> Program ExtDefList ExtDef ExtDecList Specifier StructSpecifier
OptTag  Tag VarDec  FunDec VarList ParamDec CompSt StmtList Stmt DefList Def DecList Dec Exp Args

%right ASSIGNOP
%left PLUS MINUS
%left STAR DIV
%left LP RP
%left LB RB
%left RELOP AND
%left OR DOT
%left NOT
%nonassoc ELSE
%%
Program:ExtDefList {$$=newast("Program",1,$1);if(Flag){/*printf("syntax tree:\n");print($$,0);*/initTable();Program($$);};}
    ;
ExtDefList:ExtDef ExtDefList {$$=newast("ExtDefList",2,$1,$2);}
	| {$$=newast("ExtDefList",0,-1);}
	;

ExtDef:Specifier ExtDecList SEMI    {$$=newast("ExtDef",3,$1,$2,$3);}
	|Specifier SEMI	{$$=newast("ExtDef",2,$1,$2);}
  |Specifier FunDec SEMI {$$=newast("ExtDef",3,$1,$2,$3);}
	|Specifier FunDec CompSt	{$$=newast("ExtDef",3,$1,$2,$3);}
	;

ExtDecList:VarDec {$$=newast("ExtDecList",1,$1);}
	|VarDec COMMA ExtDecList {$$=newast("ExtDecList",3,$1,$2,$3);}
	;

/*Specifier*/
Specifier:TYPE {$$=newast("Specifier",1,$1);}
	|StructSpecifier {$$=newast("Specifier",1,$1);}
	;
StructSpecifier:STRUCT OptTag LC DefList RC {$$=newast("StructSpecifier",5,$1,$2,$3,$4,$5);}
	|STRUCT Tag {$$=newast("StructSpecifier",2,$1,$2);}
	;
OptTag:ID {$$=newast("OptTag",1,$1);}
	|{$$=newast("OptTag",0,-1);}
	;
Tag:ID {$$=newast("Tag",1,$1);}
	;
/*Declarators*/
VarDec:ID {$$=newast("VarDec",1,$1);}
	| VarDec LB INT RB {$$=newast("VarDec",4,$1,$2,$3,$4);}
	| VarDec LB INT error {Flag = 0;printf("type B at Line %d\n",$3->line);}
	| error RB {Flag = 0;printf("type B at Line %d\n",$2->line);}
	;
FunDec:ID LP VarList RP {$$=newast("FunDec",4,$1,$2,$3,$4);}
	|ID LP RP {$$=newast("FunDec",3,$1,$2,$3);}
	;
VarList:ParamDec COMMA VarList {$$=newast("VarList",3,$1,$2,$3);}
	|ParamDec {$$=newast("VarList",1,$1);}
	;
ParamDec:Specifier VarDec {$$=newast("ParamDec",2,$1,$2);}
    ;

/*Statement*/
CompSt:LC DefList StmtList RC {$$=newast("Compst",4,$1,$2,$3,$4);}
	;
StmtList:Stmt StmtList{$$=newast("StmtList",2,$1,$2);}
	| {$$=newast("StmtList",0,-1);}
	;
Stmt:Exp SEMI {$$=newast("Stmt",2,$1,$2);}
	|CompSt {$$=newast("Stmt",1,$1);}
	|RETURN Exp SEMI {$$=newast("Stmt",3,$1,$2,$3);}
	|IF LP Exp RP Stmt {$$=newast("Stmt",5,$1,$2,$3,$4,$5);}
	|IF LP Exp RP Stmt ELSE Stmt {$$=newast("Stmt",7,$1,$2,$3,$4,$5,$6,$7);}
	|WHILE LP Exp RP Stmt {$$=newast("Stmt",5,$1,$2,$3,$4,$5);}
	|Exp error {Flag = 0;printf("type B at Line %d\n",$1->line);}
	|RETURN Exp error {Flag = 0;printf("type B at Line %d\n",$2->line);}
	;
/*Local Definitions*/
DefList:Def DefList{$$=newast("DefList",2,$1,$2);}
	| {$$=newast("DefList",0,-1);}
	;
Def:Specifier DecList SEMI {$$=newast("Def",3,$1,$2,$3);}
	|Specifier DecList error {Flag = 0;printf("type B at Line %d\n",$1->line);}
	;
DecList:Dec {$$=newast("DecList",1,$1);}
	|Dec COMMA DecList {$$=newast("DecList",3,$1,$2,$3);}
	;
Dec:VarDec {$$=newast("Dec",1,$1);}
	|VarDec ASSIGNOP Exp {$$=newast("Dec",3,$1,$2,$3);}
	;
/*Expressions*/
Exp:Exp ASSIGNOP Exp{$$=newast("Exp",3,$1,$2,$3);}
        |Exp AND Exp{$$=newast("Exp",3,$1,$2,$3);}
        |Exp OR Exp{$$=newast("Exp",3,$1,$2,$3);}
        |Exp RELOP Exp{$$=newast("Exp",3,$1,$2,$3);}
        |Exp PLUS Exp{$$=newast("Exp",3,$1,$2,$3);}
        |Exp MINUS Exp{$$=newast("Exp",3,$1,$2,$3);}
        |Exp STAR Exp{$$=newast("Exp",3,$1,$2,$3);}
        |Exp DIV Exp{$$=newast("Exp",3,$1,$2,$3);}
        |LP Exp RP{$$=newast("Exp",3,$1,$2,$3);}
        |MINUS Exp {$$=newast("Exp",2,$1,$2);}
        |NOT Exp {$$=newast("Exp",2,$1,$2);}
        |ID LP Args RP {$$=newast("Exp",4,$1,$2,$3,$4);}
        |ID LP RP {$$=newast("Exp",3,$1,$2,$3);}
        |Exp LB Exp RB {$$=newast("Exp",4,$1,$2,$3,$4);}
        |Exp DOT ID {$$=newast("Exp",3,$1,$2,$3);}
        |ID {$$=newast("Exp",1,$1);}
        |INT {$$=newast("Exp",1,$1);}
        |FLOAT{$$=newast("Exp",1,$1);}
	|ID LP Args error {Flag = 0;printf("type B at Line %d\n",$2->line);}
	|ID LP error {Flag = 0;printf("type B at Line %d\n",$2->line);}
	|LP Exp error {Flag = 0;printf("type B at Line %d\n",$2->line);}
	|Exp LB Exp error {Flag = 0;printf("type B at Line %d\n",$2->line);}
	|Exp LB error RB {Flag = 0;printf("type B at Line %d\n",$2->line);}
        ;
Args:Exp COMMA Args {$$=newast("Args",3,$1,$2,$3);}
        |Exp {$$=newast("Args",1,$1);}
        ;
%%
int main(int argc,char** argv)
{
if(argc<=1) return 1;
FILE * f= fopen(argv[1],"r");
if(!f)
{
perror(argv[1]);
return 1;
}
yyrestart(f);
yyparse();
if(Flag){
		lookCon();		//temps
		if(argc<=2)	return 1;
		printCode(argv[2]);
    }
return 0;
}
