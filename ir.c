#include"stdlib.h"
#include"stdio.h"
#include"string.h"
#include"ir.h"

InterCode codehead=NULL;
InterCode codetail=NULL;

int varCount=0;
int tempcount=1;
int labCount=1;


/* code node functions*/
void insertCode(InterCode c)
{
	c->prev=NULL;
	c->next=NULL;
	if(codehead==NULL){
		codehead=c;
	}
	else{
		c->prev=codetail;
		codetail->next=c;
	}
  codetail=c;
}

void deleteCode(InterCode c)
{
	if(c==codehead&&c==codetail)
	{
		codehead=NULL;
		codetail=NULL;
	}
	else if(c==codehead)
	{
		if(c->next!=NULL)
			c->next->prev=NULL;
		codehead=c->next;
	}
	else if(c==codetail)
	{
		if(c->prev!=NULL)
			c->prev->next=NULL;
		codetail=c->prev;
	}
	else
	{
		if(c->next!=NULL)
			c->next->prev=c->prev;
		if(c->prev!=NULL)
			c->prev->next=c->next;
	}
}

//print code in file
void printCode(char* fname)
{
	FILE *fp=fopen(fname,"w");
	if(fp==NULL)
	{
		printf("open file error\n");
		return;
	}
	InterCode c=codehead;
	while(c!=NULL)
	{
		switch(c->kind)
		{
			case ASSIGN_K:
				printOp(c->u.assign.left,fp);
				fputs(":=	",fp);
				printOp(c->u.assign.right,fp);
				break;
			case ADD_K:
				printOp(c->u.binop.result,fp);
				fputs(":=	",fp);
				printOp(c->u.binop.op1,fp);
				fputs("+	",fp);
				printOp(c->u.binop.op2,fp);
				break;
			case SUB_K:
				printOp(c->u.binop.result,fp);
				fputs(":=	",fp);
				printOp(c->u.binop.op1,fp);
				fputs("-	",fp);
				printOp(c->u.binop.op2,fp);
				break;
			case MUL_K:
				printOp(c->u.binop.result,fp);
				fputs(":=	",fp);
				printOp(c->u.binop.op1,fp);
				fputs("*	",fp);
				printOp(c->u.binop.op2,fp);
				break;
			case DIV_K:
				printOp(c->u.binop.result,fp);
				fputs(":=	",fp);
				printOp(c->u.binop.op1,fp);
				fputs("/	",fp);
				printOp(c->u.binop.op2,fp);
				break;
			case RETURN_K:
				fputs("RETURN	",fp);
				printOp(c->u.one.op,fp);
				break;
			case LABEL_K:
				fputs("LABEL	",fp);
				printOp(c->u.one.op,fp);
				fputs(":	",fp);
				break;
			case GOTO_K:
				fputs("GOTO	",fp);
				printOp(c->u.one.op,fp);
				break;
			case IFGOTO_K:
				fputs("IF	",fp);
				printOp(c->u.triop.t1,fp);
				fputs(c->u.triop.op,fp);
				fputs("	",fp);
				printOp(c->u.triop.t2,fp);
				fputs("GOTO	",fp);
				printOp(c->u.triop.label,fp);
				break;
			case READ_K:
				fputs("READ	",fp);
				printOp(c->u.one.op,fp);
				break;
			case WRITE_K:
				fputs("WRITE	",fp);
				printOp(c->u.one.op,fp);
				break;
			case CALL_K:
				printOp(c->u.assign.left,fp);
				fputs(":=	CALL	",fp);
				printOp(c->u.assign.right,fp);
				break;
			case ARG_K:
				fputs("ARG	",fp);
				printOp(c->u.one.op,fp);
				break;
			case FUNCTION_K:
				fputs("FUNCTION	",fp);
				printOp(c->u.one.op,fp);
				fputs(":",fp);
				break;
			case PARAM_K:
				fputs("PARAM	",fp);
				printOp(c->u.one.op,fp);
				break;
			case RIGHTAT_K:
				printOp(c->u.assign.left,fp);
				fputs(":=	&",fp);
				printOp(c->u.assign.right,fp);
				break;
			case DEC_K:
				fputs("DEC	",fp);
				printOp(c->u.dec.op,fp);
				char size[32];
				sprintf(size,"%d",c->u.dec.size);
				fputs(size,fp);
				break;
		}
		fputs("\n",fp);
		c=c->next;
	}
	fclose(fp);
}

void printOp(Operand op,FILE* fp)
{
	if(op==NULL)
	{
		fputs("t0	",fp);//t0 is use less but can avoid some error
		return;
	}
	char var[32];
	memset(var,0,sizeof(var));
	switch(op->kind)
	{
		case TEMPVAR:
			fputs("t",fp);
			sprintf(var,"%d",op->u.tmp_no);
			fputs(var,fp);
			break;
		case VARIABLE:
      fputs("v",fp);
      sprintf(var,"%d",op->u.var_no);
			fputs(var,fp);
			break;
		case CONSTANT:
			fputs("#",fp);
			fputs(op->u.value,fp);
			break;
		case LABEL:
			fputs("label",fp);
			sprintf(var,"%d",op->u.label_no);
			fputs(var,fp);
			break;
		case FUNCTION:
			fputs(op->u.funcname,fp);
			break;
		case TADDRESS: fputs("*t",fp);
			sprintf(var,"%d",op->u.name->u.var_no);
			fputs(var,fp);
			break;
		case VADDRESS:
			fputs("*",fp);
			fputs(op->u.name->u.value,fp);
			break;
	}
	fputs("	",fp);
}

/*optimize code*/
void optIF()
{
	InterCode c=codehead;
	while(c!=NULL)
	{
		if(c->kind==IFGOTO_K)
		{
			InterCode c1=c;
			InterCode c2=c->next;
			if(c2==NULL)continue;
			InterCode c3=c2->next;
			if(c3==NULL)continue;
			if(c2->kind==GOTO_K&&c3->kind==LABEL_K&&c1->u.triop.label==c3->u.one.op)
			{
				c1->u.triop.label=c2->u.one.op;
				deleteCode(c2);
				if(strcmp(c1->u.triop.op,"==")==0)
				{
					c1->u.triop.op=malloc(4);
					memset(c1->u.triop.op,0,4);
					strcpy(c1->u.triop.op,"!=");
				}
				else if(strcmp(c1->u.triop.op,"!=")==0)
				{
					c1->u.triop.op=malloc(4);
					memset(c1->u.triop.op,0,4);
					strcpy(c1->u.triop.op,"==");
				}
				else if(strcmp(c1->u.triop.op,">=")==0)
				{
					c1->u.triop.op=malloc(4);
					memset(c1->u.triop.op,0,4);
					strcpy(c1->u.triop.op,"<");
				}
				else if(strcmp(c1->u.triop.op,"<")==0)
				{
					c1->u.triop.op=malloc(4);
					memset(c1->u.triop.op,0,4);
					strcpy(c1->u.triop.op,">=");
				}
				else if(strcmp(c1->u.triop.op,">")==0)
				{
					c1->u.triop.op=malloc(4);
					memset(c1->u.triop.op,0,4);
					strcpy(c1->u.triop.op,"<=");
				}
				else if(strcmp(c1->u.triop.op,"<=")==0)
				{
					c1->u.triop.op=malloc(4);
					memset(c1->u.triop.op,0,4);
					strcpy(c1->u.triop.op,">");
				}
			}
		}
		else if(c->kind==GOTO_K)
		{
			InterCode c1=c;
			InterCode c2=c->next;
			if(c2!=NULL&&c2->kind==LABEL_K&&c1->u.one.op==c2->u.one.op)
			{
				c=c->next;
				deleteCode(c1);
			}
		}
		c=c->next;
	}
}

int opEqual(Operand op1,Operand op2)
{
	if(op1==NULL||op2==NULL)return 0;
	if(op1->kind==TEMPVAR||op1->kind==VARIABLE||op1->kind==CONSTANT);
	else	return 0;
	if(op1==op2)return 1;
	if(op1->kind==TEMPVAR&&op2->kind==TEMPVAR&&op1->u.var_no==op2->u.var_no)
		return 1;
	if(op1->kind==VARIABLE&&op2->kind==VARIABLE&&strcmp(op1->u.value,op2->u.value)==0)
		return 1;
	if(op1->kind==CONSTANT&&op2->kind==CONSTANT)
	{
		int p1=atoi(op1->u.value);
		int p2=atoi(op2->u.value);
		//printf("!!!!!%d!!!!!!%d!!!!!\n",p1,p2);
		if(p1==p2)//||op1->u.value==op2->u.value||strcpy(op1->u.value,op2->u.value)==0)
			return 1;
	}
	return 0;
}

//find constants and　figure them
void lookCon()
{
	InterCode h=codehead;
	while(h!=NULL)
	{
		if(h->kind==ADD_K||h->kind==SUB_K||h->kind==MUL_K||h->kind==DIV_K)
		{
			if(h->u.binop.result->kind==TEMPVAR&&h->u.binop.op1->kind==CONSTANT&&h->u.binop.op2->kind==CONSTANT)
			{
				int c1=atoi(h->u.binop.op1->u.value);
				int c2=atoi(h->u.binop.op2->u.value);
				int r=0;
				switch(h->kind){
					case ADD_K:r=c1+c2;break;
					case SUB_K:r=c1-c2;break;
					case MUL_K:r=c1*c2;break;
					case DIV_K:r=c1/c2;break;
					default:break;
				}
				h->u.binop.result->kind=CONSTANT;
				h->u.binop.result->u.value=malloc(32);
				sprintf(h->u.binop.result->u.value,"%d",r);
				InterCode temp=h;
				h=h->next;
				deleteCode(temp);
				continue;
			}
		}
		h=h->next;
	}
}
