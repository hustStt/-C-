#include "tmain.h"
int i;
struct tree *newast(char *name,int num,...)
{
    va_list valist;
    struct tree *a=(struct tree*)malloc(sizeof(struct tree));
    struct tree *temp=(struct tree*)malloc(sizeof(struct tree));
    if(!a)
    {
        yyerror("out of space");
        exit(0);
    }
    strcpy(a->name,name);
    va_start(valist,num);

    if(num>0)
    {
        temp=va_arg(valist, struct tree*);
        a->children=temp;
        a->line=temp->line;
        if(num>=2)
        {
            for(i=0; i<num-1; ++i)
            {
                temp->brother=va_arg(valist,struct tree*);
                temp=temp->brother;
            }
        }
    }
    else
    {
        int t=va_arg(valist, int);
        a->line=t;
        if((!strcmp(a->name,"ID"))||(!strcmp(a->name,"TYPE")))
        {strcpy(a->value,yytext);}
        else if(!strcmp(a->name,"INT")) {a->intgr=atoi(yytext);strcpy(a->value,yytext);}
        else if(!strcmp(a->name,"FLOAT")) {a->flt = atof(yytext);strcpy(a->value,yytext);}
        else if(!strcmp(a->name,"RELOP")) {strcpy(a->value,yytext);}
        else {}
    }
    return a;
}

void print(struct tree *a,int level)
{
    if(a!=NULL)
    {
        if(a->line!=-1){
            for(i=0; i<level; ++i)
              printf("  ");
            printf("%s ",a->name);
            if((!strcmp(a->name,"ID"))||(!strcmp(a->name,"TYPE")))printf(":%s ",a->value);
            else if(!strcmp(a->name,"INT"))printf(":%d",a->intgr);
            else if(!strcmp(a->name,"FLOAT")) printf(":%g",a->flt);
            else
                printf("(%d)",a->line);
            printf("\n");
        }
        print(a->children,level+1);
        print(a->brother,level);
    }
}
