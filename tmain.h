# include<stdio.h>
# include<stdlib.h>
# include<stdarg.h>
# include<string.h>
extern int yylineno;
extern char* yytext;
typedef struct tree tree;
struct tree{
	int line;			//行号
	char name[32];		//名称
	struct tree *children;		//孩子节点
	struct tree *brother;    //兄弟节点
	char value[32];					//值
  int intgr;						//整数
  float flt;						//浮点数
};

void print(struct tree *a,int level);
struct tree *newast(char *name,int num,...);
