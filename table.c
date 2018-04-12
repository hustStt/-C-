#include"table.h"
FieldList thead=NULL;  //符号表头指针
FuncList fhead=NULL;   //函数符号表头指针
int lev = 0; //当前深度
int ff = 0; //用于优化label

void insertfr(){
	FuncList fr=malloc(sizeof(struct FuncList));
	fr->name=malloc(8);
	strcpy(fr->name,"read");
	fr->isDefined=true;
	fr->line=0;
	fr->ret=malloc(sizeof(struct Type));
	fr->ret->kind=basic;
	fr->ret->u.basic=INTTYPE;
	fr->param=NULL;
	fr->next=NULL;
	insertFunc(fr,1);
}

void insertfw(){
	FuncList fw=malloc(sizeof(struct FuncList));
	fw->name=malloc(8);
	strcpy(fw->name,"write");
	fw->isDefined=true;
	fw->line=0;
	fw->ret=malloc(sizeof(struct Type));
	fw->param=malloc(sizeof(struct FieldList));
	fw->param->name=malloc(16);
	strcpy(fw->param->name,"write_param");
	fw->param->type=fw->ret;
	fw->param->tail=NULL;
	fw->param->next=NULL;
	fw->next=NULL;
	insertFunc(fw,1);
}

void initTable()
{
	thead = malloc(sizeof(struct FieldList));
	thead->next=NULL;//从头指针的下一个开始存符号 便于删除操作的进行
	insertfr();
	insertfw();
}

void printTable()   //用于打印右大括号之前的符号表
{
	int i;
	FieldList p = thead->next;
  while(p!=NULL){
		//printf("%s:",p->name);
		//printf("%d  ",p->lev2);
		p = p->next;
	}
	//printf("\n");
}

int insert(FieldList f)  //返回1表示多次定义
{
	FieldList tail = thead->next;
	if(f->name == NULL)return 0;	//不插入
	if(thead->next == NULL){
		f->lev2 = lev;
		f->count = varCount;
		varCount++;
	 	thead->next = f;
	}
	else{
		if(strcmp(tail->name,f->name)==0&&tail->lev2==lev)
			return 1;
		while(tail->next!=NULL){
			tail = tail->next;
			if(strcmp(tail->name,f->name)==0&&tail->lev2==lev)
				return 1;
		}
		f->lev2 = lev;
		f->count = varCount;
		varCount++;
		tail->next = f;
	}
	return 0;
}

void delTable()    //删除符号表中的符号
{
	FieldList p,q;
  p = thead->next;
	q = thead;
	while(p!=NULL){
		if(p->lev2 == lev){
			q->next = p->next;
			p = p->next;
		}
		else{
			p = p->next;
			q = q->next;
		}
	}
}

int insertFunc(FuncList f,int state)		//state    0:声明 1:定义
{
	FuncList q = fhead;
	if(f->name==NULL)return 0;
	if(fhead==NULL){
		fhead = f;
		insertParam(f);
	}
	else{
		if(strcmp(q->name,f->name)==0){
			if(q->isDefined)
				return 1;	//多次定义
			else{
				if(!typeEqual(q->ret,f->ret)||!paramEqual(q->param,f->param))
					return 2;	//声明不一致
				q->isDefined = state;
				return 0;
			}
		}
		while(q->next!=NULL){
			q=q->next;
			if(strcmp(q->name,f->name)==0){
				if(q->isDefined)
					return 1;	//多次定义
				else{
					if(!typeEqual(q->ret,f->ret)||!paramEqual(q->param,f->param))
						return 2;	//声明不一致
					q->isDefined=state;
					return 0;
				}
			}
		}
		q->next=f;
		insertParam(f);
	}
	return 0;
}

void insertParam(FuncList f) //将参数插入符号表
{
	lev++;
	FieldList p = f->param;
	int i=0;
	while(p!=NULL){
		i = insert(p);
		if(i == 1){
			printf("Error type 3 at line %d: Redefined variable'%s'\n",f->line,p->name);
		}
		p = p->tail;
	}
	lev--;
}

bool paramEqual(FieldList f1,FieldList f2)  //比较形参数量的以及类型是否一致
{
	if(f1==NULL&&f2==NULL)
		return true;
	if(f1==NULL||f2==NULL)
		return false;
	if(typeEqual(f1->type,f2->type))return paramEqual(f1->tail,f2->tail);
	else
		return false;
}

bool typeEqual(Type t1,Type t2)
{
	if(t1->kind==0&&t2->kind==3){
		if(t1->u.basic!=t2->u.basic){
			return false;
		}
	}
	else if(t1->kind==3&&t2->kind==0){
		if(t1->u.basic!=t2->u.basic){
			return false;
		}
	}
	else if(t1->kind!=t2->kind){
		return false;
	}
	else{
		if(t1->kind==0){//基本类型
			if(t1->u.basic!=t2->u.basic)
				return false;
		}
		else if(t1->kind==2){	//结构
			//没有名称的结构
			if(t1->u.structure->name==NULL||t2->u.structure->name==NULL){
				return paramEqual(t1->u.structure->inList,t2->u.structure->inList);
			}
			if(strcmp(t1->u.structure->name,t2->u.structure->name)!=0)
				return paramEqual(t1->u.structure->inList,t2->u.structure->inList);
		}
		else if(t1->kind==1){//数组
			return typeEqual(t1->u.array.elem,t2->u.array.elem);
		}
	}
	return true;
}

FieldList findF(char *name){
	FieldList tail = thead->next;
	if(tail == NULL)return NULL;
	int lev3;
	for(lev3 = lev;lev3 >= 0;lev3--){
		tail = thead->next;
		while(tail!=NULL){
			if(strcmp(tail->name,name)==0&&tail->lev2==lev3)
				return tail;
			tail=tail->next;
			}
	}
	return NULL;
}

FuncList findFunc(char *name)
{
	FuncList tail = fhead;
	if(tail == NULL)return NULL;
	while(tail!=NULL){
		if(strcmp(tail->name,name)==0)return tail;
		tail=tail->next;
	}
	return NULL;
}

void checkFunc()   //检查声明但没有定义的函数
{
	int i;
	FuncList tail = fhead;
	while(tail!=NULL){
		if(!tail->isDefined)
			printf("Error type 18 at line %d: Undefined function '%s'\n",tail->line,tail->name);
		tail=tail->next;
	}
}
/* 遍历*/
void Program(tree *n)
{
	ExtDefList(n->children);
	checkFunc();
}

void ExtDefList(tree *n)
{
	tree *child=n->children;
	if(child!=NULL){
		ExtDef(child);
		ExtDefList(child->brother);
	}
}

/*definition of variables*/
void ExtDef(tree *n){
	tree *child = n->children;
	Type type;
	type = Specifier(child);
	child = child->brother;
	if(strcmp(child->name,"SEMI")==0)
		return;
	else if(strcmp(child->name,"ExtDecList")==0)
	{
		ExtDecList(child,type);
	}
	else
	{
		FuncList f=FunDec(child,type);
		child=child->brother;
		int i=0;
		if(strcmp(child->name,"SEMI")==0)
		{
			if(f==NULL)return;
			f->isDefined=0;  //声明
			i=insertFunc(f,0);
			if(i==1)
				printf("Error type 4 at line %d: Redefined function '%s'\n",f->line,f->name);
			else if(i==2)
				printf("Error type 19 at line %d: Inconsistent declaration of function '%s'\n",f->line,f->name);
		}
		else{
			if(f!=NULL){
				f->isDefined=1;  //定义
				i=insertFunc(f,1);
				if(i==1)
					printf("Error type 4 at line %d: Redefined function '%s'\n",f->line,f->name);
				else if(i==2)
					printf("Error type 19 at line %d: Inconsistent declaration of function '%s'\n",f->line,f->name);
				else{
					trExtDef(f);
					}
			}
			CompSt(child,type);		//给返回类型
		}
	}
}

void trExtDef(FuncList f){
	Operand funcop=malloc(sizeof(struct Operand_));
	funcop->kind=FUNCTION;
	funcop->u.funcname=f->name;
	InterCode code=malloc(sizeof(struct InterCode));
	code->kind=FUNCTION_K;
	code->u.one.op=funcop;
	insertCode(code);		//funtion  :
	FieldList param=f->param;
	while(param!=NULL){
		Operand pop=malloc(sizeof(struct Operand_));	//king of pop--MJ
		pop->kind=VARIABLE;
		pop->u.tmp_no=param->count;
		InterCode pcode=malloc(sizeof(struct InterCode));
		pcode->kind=PARAM_K;
		pcode->u.one.op=pop;
		insertCode(pcode);
		param=param->tail;
		}
}

void ExtDecList(tree *n,Type type)
{
	tree *child = n->children;
	FieldList f = VarDec(child,type,1);
	child = child->brother;
	if(child != NULL)
	{
		child = child->brother;
		ExtDecList(child,type);
	}
}

/*Specifiers*/
Type Specifier(tree* n)
{
	tree *child=n->children;
	Type t;
	if(strcmp(child->name,"TYPE")==0){
		t = malloc(sizeof(struct Type));
		t->kind = 0;
		if(strcmp(child->value,"int")==0){
			t->u.basic=INTTYPE;   //0
		}
		else if(strcmp(child->value,"float")==0){
			t->u.basic=FLOATTYPE;   //1
		}
	}
	else{
		t = StructSpecifier(child);
	}
	return t;
}

//struct type
Type StructSpecifier(tree *n)
{
	tree *child=n->children;
	Type type=malloc(sizeof(struct Type));
	type->kind=2;
	type->u.structure=malloc(sizeof(struct Structure));
	type->u.structure->inList=NULL;
	FieldList f=malloc(sizeof(struct FieldList));
	f->type=type;
	while(child!=NULL)
	{
		if(strcmp(child->name,"OptTag")==0)
		{
			if(child->children==NULL)
			{
				type->u.structure->name=NULL;
			}
			else{
				type->u.structure->name = malloc(sizeof(child->children->value));
				strcpy(type->u.structure->name,child->children->value);
				f->name=malloc(sizeof(child->children->value));
				strcpy(f->name,child->children->value);
			}
		}
		else if(strcmp(child->name,"Tag")==0)
		{
			FieldList structf = findF(child->children->value);
			if(structf==NULL||structf->type->kind!=2||strcmp(structf->name,structf->type->u.structure->name)!=0)
			{
				printf("Error type 17 at line %d: Undefined struct ‘%s’\n",child->line,child->children->value);
				return NULL;
			}
			return structf->type;
		}
		else if(strcmp(child->name,"DefList")==0)
		{
			lev++;
			type->u.structure->inList=DefList(child,2);	//struct field
			//printf("struct  ");
			printTable();
			delTable(lev);
			lev--;
			if(f->type->u.structure->name==NULL)return type;	//s struct without name
			int i=insert(f);
			if(i==1)
			{
				printf("Error type 16 at line %d: Duplicated name ‘%s’\n",child->line,f->name);
				return NULL;
			}
			return type;
		}
		child=child->brother;
	}
}

/*Declarators*/
FieldList VarDec(tree *n,Type type,int stype)		//error type 3   stype 2:struct
	//redefined variable
{
	tree *child=n->children;
	FieldList f;
	if(strcmp(child->name,"ID")==0)
	{
		f=malloc(sizeof(struct FieldList));
		f->name=malloc(sizeof(child->value));
		strcpy(f->name,child->value);
		f->type=type;
		f->tail=NULL;
		f->next=NULL;
		if(stype==3)return f;	//do not insert this  param now
		int i=insert(f);		//insert this ID
		//if redefined
		if(i==1)
		{
			if(stype==1)
				printf("Error type 3 at line %d: Redefined variable'%s'\n",child->line,f->name);
			else
				printf("Error type 15 at line %d: Redefined field ‘%s’\n",child->line,f->name);
			return NULL;
		}
		if(stype==1&&type->kind==2)		//DEC code
		{
			Operand op=malloc(sizeof(struct Operand_));
			op->kind=TEMPVAR;
			op->u.tmp_no=tempcount++;

			InterCode deccode=malloc(sizeof(struct InterCode));
			deccode->kind=DEC_K;
			deccode->u.dec.op=op;
			deccode->u.dec.size=typeSize(type);
			insertCode(deccode);

			Operand v=malloc(sizeof(struct Operand_));
			v->kind=VARIABLE;
			v->u.value=child->value;

			InterCode addrcode=malloc(sizeof(struct InterCode));
			addrcode->kind=RIGHTAT_K;
			addrcode->u.assign.left=v;
			addrcode->u.assign.right=op;
			insertCode(addrcode);
		}
		return f;
	}
	else
	{							//construct arry
		f=VarDec(child,type,stype);
		if(f==NULL)return NULL;
		Type temp=f->type;
		child=child->brother;
		child=child->brother;	//INT
		Type t=malloc(sizeof(struct Type));
		t->kind=1;		//array
		t->u.array.size=atoi(child->value);
		t->u.array.elem=type;
		if(temp->kind!=1)
		{
			f->type=t;
			return f;
		}
		while(temp->u.array.elem->kind==1)	//find the end of this array
		{
			temp=temp->u.array.elem;
		}
		temp->u.array.elem=t;
		//printtype(f->type);
		return f;
	}
}

FuncList FunDec(tree* n,Type type)
{
	tree* child=n->children;
	FuncList f=malloc(sizeof(struct FuncList));
	f->name=malloc(sizeof(child->value));
	strcpy(f->name,child->value);
	f->isDefined=0;
	f->line=child->line;
	f->ret=type;
	f->next=NULL;
	f->param=NULL;
	child=child->brother;
	child=child->brother;
	if(strcmp(child->name,"VarList")==0)
	{
		f->param=VarList(child);
	}
	return f;
}

FieldList VarList(tree *n)
{
	tree* child=n->children;
	FieldList f;
	f=ParamDec(child);
	child=child->brother;
	if(child!=NULL)
	{
		FieldList p=f;
		child=child->brother;
		if(p==NULL)f=VarList(child);
		else{
			while(p->tail!=NULL)p=p->tail;
			p->tail=VarList(child);
		}
	}
	return f;
}

FieldList ParamDec(tree*n)
{
	tree *child=n->children;
	FieldList f;
	Type type;
	type=Specifier(child);
	f=VarDec(child->brother,type,3);		//from function param
	return f;
}

/*Statements*/
void CompSt(tree * n,Type ret)
{
	lev++;
	tree *child=n->children;
	child=child->brother;
	DefList(child,1);
	child=child->brother;
	StmtList(child,ret);
	//printf("func  ");
	printTable();
	delTable(lev);
	lev--;
}

void StmtList(tree *n,Type ret)
{
	tree *child=n->children;
	if(child==NULL)return;
	Stmt(child,ret);
	child=child->brother;
	StmtList(child,ret);
}

void Stmt(tree *n,Type ret)		//error type 8	return //return type mismatched
{
	//printName(n->name);
	//TODO:more details
	ff = 0;
	tree*child=n->children;
	if(child==NULL)return;
	if(strcmp(child->name,"Exp")==0)
	{
		Exp(child,NULL);
		return;
	}
	else if(strcmp(child->name,"CompSt")==0)
	{
		CompSt(child,ret);
		return;
	}
	else if(strcmp(child->name,"RETURN")==0)
	{
		child=child->brother;
		//new temp
		ff = 1;
		Operand op=malloc(sizeof(struct Operand_));
		op->kind=TEMPVAR;
		op->u.tmp_no=tempcount++;
		Type t=Exp(child,op);
		if(ret==NULL||t==NULL)return;
		if(!typeEqual(ret,t))
		{
			printf("Error type 8 at line %d: The return type mismatched\n",child->line);
			return;
		}
		InterCode code=malloc(sizeof(struct InterCode));
		code->kind=RETURN_K;
		code->u.one.op=op;
		insertCode(code);
		return;
	}
	else if(strcmp(child->name,"IF")==0)
	{
		child=child->brother->brother;
		//new temp
		Operand lb1=malloc(sizeof(struct Operand_));
		lb1->kind=LABEL;
		lb1->u.label_no=labCount++;
		Operand lb2=malloc(sizeof(struct Operand_));
		lb2->kind=LABEL;
		lb2->u.label_no=labCount++;
		Type t=Exp_Cond(child,lb1,lb2);	//TODO:this function
		if(t!=NULL&&!((t->kind==0||t->kind==3)&&t->u.basic==INTTYPE))
		{
			printf("Error type ? conditional statement wrong type\n");
		}
		//print label1
		InterCode code1=malloc(sizeof(struct InterCode));
		code1->kind=LABEL_K;
		code1->u.one.op=lb1;
		insertCode(code1);
		child=child->brother->brother;
		Stmt(child,ret);//code2

		InterCode lb2code=malloc(sizeof(struct InterCode));
		lb2code->kind=LABEL_K;
		lb2code->u.one.op=lb2;
		if(child->brother!=NULL)
		{
			Operand lb3=malloc(sizeof(struct Operand_));
			lb3->kind=LABEL;
			if(!ff)lb3->u.label_no=labCount++;
			InterCode code2=malloc(sizeof(struct InterCode));
			code2->kind=GOTO_K;
			code2->u.one.op=lb3;
			if(!ff)insertCode(code2);			//goto label3
			insertCode(lb2code);		//label2
			child=child->brother->brother;
			Stmt(child,ret);			//code3
			InterCode lb3code=malloc(sizeof(struct InterCode));
			lb3code->kind=LABEL_K;
			lb3code->u.one.op=lb3;
			if(!ff)insertCode(lb3code);		//label3
		}
		else
			insertCode(lb2code);
	}
	else if(strcmp(child->name,"WHILE")==0)
	{
		Operand lb1=malloc(sizeof(struct Operand_));
		lb1->kind=LABEL;
		lb1->u.label_no=labCount++;
		Operand lb2=malloc(sizeof(struct Operand_));
		lb2->kind=LABEL;
		lb2->u.label_no=labCount++;
		Operand lb3=malloc(sizeof(struct Operand_));
		lb3->kind=LABEL;
		lb3->u.label_no=labCount++;
		child=child->brother->brother;

		InterCode lb1code=malloc(sizeof(struct InterCode));
		lb1code->kind=LABEL_K;
		lb1code->u.one.op=lb1;
		insertCode(lb1code);		//label 1
		Type t=Exp_Cond(child,lb2,lb3);	//code1
		if(t!=NULL&&!((t->kind==0||t->kind==3)&&t->u.basic==INTTYPE))
		{
			printf("Error type ? conditional statement wrong type\n");
		}

		InterCode lb2code=malloc(sizeof(struct InterCode));
		lb2code->kind=LABEL_K;
		lb2code->u.one.op=lb2;
		insertCode(lb2code);		//label 2
		child=child->brother->brother;
		Stmt(child,ret);			//code2
		InterCode gotolb1=malloc(sizeof(struct InterCode));
		gotolb1->kind=GOTO_K;
		gotolb1->u.one.op=lb1;
		insertCode(gotolb1);		//goto label1
		InterCode lb3code=malloc(sizeof(struct InterCode));
		lb3code->kind=LABEL_K;
		lb3code->u.one.op=lb3;
		insertCode(lb3code);		//label3
	}
}

/*Local Definitions*/
FieldList DefList(tree* n,int stype)
{
	if(n->children==NULL)return NULL;
	FieldList f;
	tree *child=n->children;
	f=Def(child,stype);
	FieldList t=f;
	child=child->brother;
	if(t!=NULL){
		while(t->tail!=NULL)	//find the last field
		{
			t=t->tail;
		}
		t->tail=DefList(child,stype);
	}
	else f=DefList(child,stype);
	return f;
}

FieldList Def(tree *n,int stype)
{
	tree* child=n->children;
	FieldList f;	//=malloc(sizeof(struct FieldList_));
	Type type=Specifier(child);
	child=child->brother;
	f=DecList(child,type,stype);	//inhre
	return f;
}

FieldList DecList(tree *n,Type type,int stype)
{
	tree *child=n->children;
	FieldList f;
	f=Dec(child,type,stype);
	child=child->brother;
	if(child!=NULL){
		child=child->brother;
		FieldList p=f;
		if(p!=NULL)
		{
			while(p->tail!=NULL)p=p->tail;
			p->tail=DecList(child,type,stype);
		}
		else
			f=DecList(child,type,stype);
	}
	return f;
}

FieldList Dec(tree *n,Type type,int stype)	//error type 5
	//type mismatched
{//printName(n->name);
	tree *child=n->children;
	FieldList f;
	f=VarDec(child,type,stype);

	if(f->type->kind==1&&stype==1)
	{
		//array space
		Operand op=malloc(sizeof(struct Operand_));
		op->kind=TEMPVAR;
		op->u.tmp_no=tempcount++;

		InterCode deccode=malloc(sizeof(struct InterCode));
		deccode->kind=DEC_K;
		deccode->u.dec.op=op;
		deccode->u.dec.size=typeSize(f->type);
		insertCode(deccode);

		Operand v=malloc(sizeof(struct Operand_));
		v->kind=VARIABLE;
		v->u.var_no=f->count;

		InterCode addrcode=malloc(sizeof(struct InterCode));
		addrcode->kind=RIGHTAT_K;
		addrcode->u.assign.left=v;
		addrcode->u.assign.right=op;
		insertCode(addrcode);
	}

	if(f==NULL)return NULL;
	child=child->brother;
	if(child!=NULL){		//assignop =
		if(stype==2){	//struct cannot be initialized
			printf("Error type 15 at line %d: be initialized field ‘%s’\n",child->line,f->name);
			return f;
		}
		Operand place=malloc(sizeof(struct Operand_));
		place->kind=VARIABLE;
		place->u.var_no=f->count;
		child=child->brother;
		Type t=Exp(child,place);
		if(t!=NULL&&type!=NULL&&!typeEqual(type,t))
		{
			printf("Error type 5 at line %d: The type mismatched\n",child->line);
		}
		if(place->kind!=VARIABLE||place->u.var_no!=f->count)
		{
			Operand left=malloc(sizeof(struct Operand_));
			left->kind=VARIABLE;
			left->u.var_no=f->count;

			InterCode asscode=malloc(sizeof(struct InterCode));
			asscode->kind=ASSIGN_K;
			asscode->u.assign.left=left;
			asscode->u.assign.right=place;
			insertCode(asscode);
		}
	}
	return f;
}

/*Expressions*/
Type Exp(tree *n,Operand place)
{//printName(n->name);
	tree *child=n->children;
	if(strcmp(child->name,"Exp")==0)
	{
		tree *child2=child->brother;
		//=
		if(strcmp(child2->name,"ASSIGNOP")==0)
		{
			//left value
			tree *leftChild=child->children;
			Type leftType=NULL;
			//new temp
			Operand leftOp=malloc(sizeof(struct Operand_));
			leftOp->kind=TEMPVAR;
			leftOp->u.tmp_no=tempcount++;
			//because of associative property and priority,it is right
			if(strcmp(leftChild->name,"ID")==0&&leftChild->brother==NULL)
				leftType=Exp(child,leftOp);
			else if(strcmp(leftChild->name,"Exp")==0&&leftChild->brother!=NULL&&strcmp(leftChild->brother->name,"LB")==0)	//array
				leftType=Exp(child,leftOp);
			else if(strcmp(leftChild->name,"Exp")==0&&leftChild->brother!=NULL&&strcmp(leftChild->brother->name,"DOT")==0)	//struct
				leftType=Exp(child,leftOp);
			else
			{
				printf("Error type 6 at line %d: The left-hand side of an assignment must be a variable\n",child->line);
				return NULL;
			}

			child2=child2->brother;
			//new temp
			Operand rightOp=malloc(sizeof(struct Operand_));
			rightOp->kind=TEMPVAR;
			rightOp->u.tmp_no=tempcount++;
			int temp_no=rightOp->u.var_no;		//trick
			Type rightType=Exp(child2,rightOp);

			if(leftType==NULL||rightType==NULL)return NULL;
			if(typeEqual(leftType,rightType))
			{
				//print code here
				if(rightOp->kind==TEMPVAR&&rightOp->u.var_no==temp_no&&(leftOp->kind==TEMPVAR||leftOp->kind==VARIABLE))
					memcpy(rightOp,leftOp,sizeof(struct Operand_));
				else
				{
					InterCode code1=malloc(sizeof(struct InterCode));
					code1->kind=ASSIGN_K;
					code1->u.assign.left=leftOp;
					code1->u.assign.right=rightOp;
					insertCode(code1);
				}
				InterCode code2=malloc(sizeof(struct InterCode));
				code2->kind=ASSIGN_K;
				code2->u.assign.left=place;
				code2->u.assign.right=rightOp;
				if(place!=NULL)
					insertCode(code2);
				return leftType;
			}
			else
			{
				printf("Error type 5 at line %d: Type mismatched\n",child->line);
				return NULL;
			}
		}
		else if(strcmp(child2->name,"PLUS")==0||strcmp(child2->name,"MINUS")==0||strcmp(child2->name,"STAR")==0||strcmp(child2->name,"DIV")==0)		//+ - * /
		{
			//new temp
			Operand op1=malloc(sizeof(struct Operand_));
			op1->kind=TEMPVAR;
			op1->u.tmp_no=tempcount++;
			Type t=Exp(child,op1);

			//new temp
			Operand op2=malloc(sizeof(struct Operand_));
			op2->kind=TEMPVAR;
			op2->u.tmp_no=tempcount++;
			tree *child3=child2;
			child2=child2->brother;
			Type t2=Exp(child2,op2);
			if(t==NULL||t2==NULL)return NULL;
			else if((t->kind==0||t->kind==3)&&(t2->kind==0||t2->kind==3)&&t->u.basic==t2->u.basic)
			{
				if(place==NULL)return t;
				InterCode code=malloc(sizeof(struct InterCode));
				code->u.binop.result=place;
				code->u.binop.op1=op1;
				code->u.binop.op2=op2;
				if(strcmp(child3->name,"PLUS")==0)
					code->kind=ADD_K;
				else if(strcmp(child3->name,"MINUS")==0)
					code->kind=SUB_K;
				else if(strcmp(child3->name,"STAR")==0)
					code->kind=MUL_K;
				else if(strcmp(child3->name,"DIV")==0)
					code->kind=DIV_K;
				if(place!=NULL)
					insertCode(code);
				return t;
			}
			else
			{
				printf("Error type 7 at line %d: Operands type mismatched\n",child->line);
				return NULL;
			}
		}
		else if(strcmp(child2->name,"AND")==0||strcmp(child2->name,"OR")==0||strcmp(child2->name,"RELOP")==0)//and or /
		{
			//new temp
			Operand lb1=malloc(sizeof(struct Operand_));
			lb1->kind=LABEL;
			lb1->u.label_no=labCount++;
			Operand lb2=malloc(sizeof(struct Operand_));
			lb2->kind=LABEL;
			lb2->u.label_no=labCount++;

			if(place!=NULL){
				InterCode code0=malloc(sizeof(struct InterCode));
				code0->kind=ASSIGN_K;
				code0->u.assign.left=place;
				Operand c0=malloc(sizeof(struct Operand_));
				c0->kind=CONSTANT;
				c0->u.value=malloc(32);
				strcpy(c0->u.value,"0");
				code0->u.assign.right=c0;
				insertCode(code0);	//code0
			}
			Type t=Exp_Cond(n,lb1,lb2);	//code1

			InterCode lb1code=malloc(sizeof(struct InterCode));
			lb1code->kind=LABEL_K;
			lb1code->u.one.op=lb1;
			insertCode(lb1code);	//label 1

			Operand c1=malloc(sizeof(struct Operand_));
			c1->kind=CONSTANT;
			c1->u.value=malloc(32);
			strcpy(c1->u.value,"1");
			InterCode code2=malloc(sizeof(struct InterCode));
			code2->kind=ASSIGN_K;
			code2->u.assign.left=place;
			code2->u.assign.right=c1;
			if(place!=NULL)
				insertCode(code2);		//code2

			InterCode lb2code=malloc(sizeof(struct InterCode));
			lb2code->kind=LABEL_K;
			lb2code->u.one.op=lb2;
			insertCode(lb2code);
			return t;
		}
		else if(strcmp(child2->name,"LB")==0)	//array
		{
			Operand aop=malloc(sizeof(struct Operand_));
			aop->kind=TEMPVAR;
			aop->u.tmp_no=tempcount++;
			Type t1=Exp(child,aop);	//array
			//child's children must be a ID,
			if(t1==NULL)return NULL;
			if(t1->kind!=1)
			{
				printf("Error type 10 at line %d: '",child->line);
				printNode(child);
				printf("' must be an array\n");
				return NULL;
			}

			Operand subs=malloc(sizeof(struct Operand_));
			subs->kind=TEMPVAR;
			subs->u.tmp_no=tempcount++;

			child2=child2->brother;
			Type t2=Exp(child2,subs);
			//printf("array back\n");
			if(t2==NULL)return NULL;
			if(!((t2->kind==0||t2->kind==3)&&t2->u.basic==INTTYPE))
			{
				printf("Error type 12 at line %d: Operands type mistaken\n",child2->line);
				return NULL;
			}

			Operand offset=malloc(sizeof(struct Operand_));
			offset->kind=TEMPVAR;
			offset->u.tmp_no=tempcount++;

			Operand intsize=malloc(sizeof(struct Operand_));
			intsize->kind=CONSTANT;
			intsize->u.value=malloc(32);
			memset(intsize->u.value,0,32);
			Type subtype=t1->u.array.elem;
			sprintf(intsize->u.value,"%d",typeSize(subtype));

			InterCode addrcode=malloc(sizeof(struct InterCode));
			addrcode->kind=MUL_K;
			addrcode->u.binop.result=offset;
			addrcode->u.binop.op1=subs;
			addrcode->u.binop.op2=intsize;
			insertCode(addrcode);

			Operand temp=malloc(sizeof(struct Operand_));
			temp->kind=TEMPVAR;
			temp->u.tmp_no=tempcount++;

			InterCode code1=malloc(sizeof(struct InterCode));
			code1->kind=ADD_K;

			if(subtype->kind==0){
				place->kind=TADDRESS;
				place->u.name=temp;
				code1->u.binop.result=temp;
			}
			else
				code1->u.binop.result=place;

			//if(aop->kind==VADDRESS)
			//	aop->kind=VARIABLE;
			//else if(aop->kind==TADDRESS)
			//	aop->kind=TEMPVAR;
			code1->u.binop.op1=aop;
			code1->u.binop.op2=offset;
			insertCode(code1);

			return t1->u.array.elem;
		}
		else if(strcmp(child2->name,"DOT")==0)	//struct
		{
			Operand op1=malloc(sizeof(struct Operand_));
			op1->kind=TEMPVAR;
			op1->u.tmp_no=tempcount++;
			Type t1=Exp(child,op1);
			if(t1==NULL)return NULL;
			if(t1->kind!=2)
			{
				printf("Error type 13 at line %d: Illegal use of '.'\n",child->line);
				return NULL;
			}
			FieldList fl=t1->u.structure->inList;
			child2=child2->brother;
			int size=0;
			while(fl!=NULL)
			{
				if(strcmp(fl->name,child2->value)==0)
				{
					if(size==0)
					{
						if(op1->kind==VARIABLE||op1->kind==VADDRESS)
						{
							if(fl->type->kind==0)
							{
								place->kind=VADDRESS;
								place->u.name=op1;
							}
							else
							{
								place->kind=VARIABLE;
								place->u.var_no=op1->u.var_no;
							}
						}
						else if(op1->kind==TEMPVAR||op1->kind==TADDRESS)
						{
							if(fl->type->kind==0)
							{
								place->kind=TADDRESS;
								place->u.name=op1;
							}
							else
							{
								place->kind=TEMPVAR;
								place->u.var_no=op1->u.var_no;
							}
						}
					}
					else
					{
						Operand temp=malloc(sizeof(struct Operand_));
						temp->kind=TEMPVAR;
						temp->u.tmp_no=tempcount++;

						Operand conOp=malloc(sizeof(struct Operand_));
						conOp->kind=CONSTANT;
						conOp->u.value=malloc(32);
						memset(conOp->u.value,0,32);
						sprintf(conOp->u.value,"%d",size);

						InterCode code1=malloc(sizeof(struct InterCode));
						code1->kind=ADD_K;

						if(fl->type->kind==0)
						{
							place->kind=TADDRESS;
							place->u.name=temp;
							code1->u.binop.result=temp;
						}
						else
							code1->u.binop.result=place;
						//if(op1->kind==TADDRESS)
						//	op1->kind=TEMPVAR;
						//else if(op1->kind==VADDRESS)
						//	op1->kind=VARIABLE;
						code1->u.binop.op1=op1;
						code1->u.binop.op2=conOp;
						insertCode(code1);
					}
					return fl->type;
				}
				size+=typeSize(fl->type);
				fl=fl->tail;
			}
			printf("Error type 14 at line %d: Un-existed field '%s‘\n",child2->line,child2->value);
			return NULL;
		}

	}
	else if(strcmp(child->name,"LP")==0)	//()
	{
		child=child->brother;
		return Exp(child,place);
	}
	else if(strcmp(child->name,"MINUS")==0)	//-
	{
		//new temp
		child=child->brother;
		Operand op=malloc(sizeof(struct Operand_));
		op->kind=TEMPVAR;
		op->u.tmp_no=tempcount++;
		Type t=Exp(child,op);
		if(t==NULL)return NULL;
		if(t->kind!=0&&t->kind!=3)
		{
			printf("Error type 7 at line %d: Operands type mismatched1\n",child->line);
			return NULL;
		}

		//print code here
		Operand op2=malloc(sizeof(struct Operand_));
		op2->kind=CONSTANT;
		op2->u.value=malloc(4);
		strcpy(op2->u.value,"0");
		InterCode code=malloc(sizeof(struct InterCode));
		code->kind=SUB_K;
		code->u.binop.result=place;
		code->u.binop.op1=op2;
		code->u.binop.op2=op;
		if(place!=NULL)
			insertCode(code);

		return t;
	}
	else if(strcmp(child->name,"NOT")==0)	//not
	{
		//child=child->brother;
		//Type t=Exp(child);
		//if(t==NULL)return NULL;
		//if(t->kind==0&&t->u.basic==INTTYPE)return t;
		//printf("Error type 7 at line %d: Operands type mismatched\n",child->line);
		//return NULL;
		//new temp
		Operand lb1=malloc(sizeof(struct Operand_));
		lb1->kind=LABEL;
		lb1->u.label_no=labCount++;
		Operand lb2=malloc(sizeof(struct Operand_));
		lb2->kind=LABEL;
		lb2->u.label_no=labCount++;

		InterCode code0=malloc(sizeof(struct InterCode));
		code0->kind=ASSIGN_K;
		code0->u.assign.left=place;
		Operand c0=malloc(sizeof(struct Operand_));
		c0->kind=CONSTANT;
		c0->u.value=malloc(32);
		strcpy(c0->u.value,"0");
		code0->u.assign.right=c0;
		if(place!=NULL)
			insertCode(code0);	//code0
		Type t=Exp_Cond(n,lb1,lb2);	//code1

		InterCode lb1code=malloc(sizeof(struct InterCode));
		lb1code->kind=LABEL_K;
		lb1code->u.one.op=lb1;
		insertCode(lb1code);	//label 1

		Operand c1=malloc(sizeof(struct Operand_));
		c1->kind=CONSTANT;
		c1->u.value=malloc(32);
		strcpy(c1->u.value,"1");
		InterCode code2=malloc(sizeof(struct InterCode));
		code2->kind=ASSIGN_K;
		code2->u.assign.left=place;
		code2->u.assign.right=c1;
		if(place!=NULL)
			insertCode(code2);		//code2
InterCode lb2code=malloc(sizeof(struct InterCode)); lb2code->kind=LABEL_K; lb2code->u.one.op=lb2;
		insertCode(lb2code);
		return t;
	}
	else if(strcmp(child->name,"ID")==0&&child->brother!=NULL)	//func
	{

		FieldList f1=findF(child->value);
		FuncList f=findFunc(child->value);
		if(f1!=NULL&&f==NULL)
		{
			printf("Error type 11 at line %d: '%s' must be a function\n",child->line,child->value);
			return NULL;
		}
		if(f==NULL||!f->isDefined){
			printf("Error type 2 at line %d: Undefined function '%s'\n",child->line,child->value);
			return NULL;
		}
		FieldList param=f->param;
		child=child->brother;
		child=child->brother;
		if(strcmp(child->name,"RP")==0)
		{
			if(param!=NULL)
			{
				printf("Error type 9 at line%d : The method '%s(",child->line,f->name);
				printparam(param);
				printf(")'is not applicable for the arguments '()'\n");
			}
			if(strcmp(f->name,"read")==0)
			{
				InterCode rpcode=malloc(sizeof(struct InterCode));
				rpcode->kind=READ_K;
				rpcode->u.one.op=place;
				if(place!=NULL)
					insertCode(rpcode);		//read place
			}
			else
			{
				Operand fop=malloc(sizeof(struct Operand_));
				fop->kind=FUNCTION;
				fop->u.funcname=f->name;
				InterCode cfcode=malloc(sizeof(struct InterCode));
				cfcode->kind=CALL_K;
				cfcode->u.assign.left=place;//TODO:NULL?
				cfcode->u.assign.right=fop;
				insertCode(cfcode);			//call func
			}
		}
		else
		{
			Operand arg_list_head=malloc(sizeof(struct Operand_));
			arg_list_head->next=NULL;
			if(!Args(child,param,arg_list_head)){
				printf("Error type 9 at line%d : The method '%s(",child->line,f->name);
				printparam(param);
				printf(")'is not applicable for the arguments '(");
				//printargs(child);
				printf(")'\n");
			}
			else
			{
				if(strcmp(f->name,"write")==0)
				{
					InterCode wacode=malloc(sizeof(struct InterCode));
					wacode->kind=WRITE_K;
					wacode->u.one.op=arg_list_head->next;
					insertCode(wacode);			//write arg
				}
				else
				{
					arg_list_head=arg_list_head->next;
					while(arg_list_head!=NULL)
					{
						InterCode argcode=malloc(sizeof(struct InterCode));
						argcode->kind=ARG_K;
						argcode->u.one.op=arg_list_head;
						insertCode(argcode);		//Arg arg
						arg_list_head=arg_list_head->next;
					}
					Operand fop=malloc(sizeof(struct Operand_));
					fop->kind=FUNCTION;
					fop->u.funcname=f->name;
					InterCode cfcode=malloc(sizeof(struct InterCode));
					cfcode->kind=CALL_K;
					cfcode->u.assign.left=place;
					cfcode->u.assign.right=fop;
					insertCode(cfcode);		//call func
				}
			}
		}
		return f->ret;
	}
	else if(strcmp(child->name,"ID")==0)
	{
		FieldList f=findF(child->value);
		if(f==NULL)
		{
			printf("Error type 1 at line %d: Undefined variable '%s'\n",child->line,child->value);
			return NULL;
		}

		//print code here
		place->kind=VARIABLE;
		place->u.var_no=f->count;

		return f->type;
	}
	else if(strcmp(child->name,"INT")==0)
	{
		Type t=malloc(sizeof(struct Type));
		t->kind=3;
		t->u.basic=INTTYPE;
		//print code here
		if(place!=NULL)
		{
			place->kind=CONSTANT;
			place->u.value=child->value;
		}
		return t;
	}
	else if(strcmp(child->name,"FLOAT")==0)
	{
		Type t=malloc(sizeof(struct Type));
		t->kind=3;
		t->u.basic=FLOATTYPE;
		if(place!=NULL)
		{
			place->kind=CONSTANT;
			place->u.value=child->value;
		}
		return t;
	}
	return NULL;
}

//exp condition
Type Exp_Cond(tree *n,Operand label_true,Operand label_false)
{//printName(n->name);
	tree *child=n->children;
	Type type;
	if(strcmp(child->name,"Exp")==0)
	{
		tree *child2=child->brother;
		if(strcmp(child2->name,"RELOP")==0)//< >
		{
			//new temp
			Operand t1=malloc(sizeof(struct Operand_));
			t1->kind=TEMPVAR;
			t1->u.tmp_no=tempcount++;
			Operand t2=malloc(sizeof(struct Operand_));
			t2->kind=TEMPVAR;
			t2->u.tmp_no=tempcount++;

			tree* child3=child2;
			child2=child2->brother;
			Type tp=Exp(child,t1);	//code1
			Type tp2=Exp(child2,t2);	//code2
			if(tp==NULL||tp2==NULL)return NULL;
			else if((tp->kind==0||tp->kind==3)&&(tp2->kind==0||tp2->kind==3)&&tp->u.basic==tp2->u.basic)
			{
				InterCode code3=malloc(sizeof(struct InterCode));
				code3->kind=IFGOTO_K;
				code3->u.triop.t1=t1;
				code3->u.triop.op=child3->value;
				code3->u.triop.t2=t2;
				code3->u.triop.label=label_true;
				insertCode(code3);		//code3

				InterCode gotolbf=malloc(sizeof(struct InterCode));
				gotolbf->kind=GOTO_K;
				gotolbf->u.one.op=label_false;
				insertCode(gotolbf);		//goto label false
				return tp;
			}
			else
			{
				printf("Error type 7 at line %d: Operands type mismatched!\n",child->line);
				return NULL;
			}

		}
		else if(strcmp(child2->name,"AND")==0)
		{
			//new temp
			Operand lb1=malloc(sizeof(struct Operand_));
			lb1->kind=LABEL;
			lb1->u.label_no=labCount++;

			Type t=Exp_Cond(child,lb1,label_false);	//code1

			InterCode lb1code=malloc(sizeof(struct InterCode));
			lb1code->kind=LABEL_K;
			lb1code->u.one.op=lb1;
			insertCode(lb1code);		//label 1

			child2=child2->brother;
			Type t2=Exp_Cond(child2,label_true,label_false);	//code2
			if(t==NULL||t2==NULL)return NULL;
			else if((t->kind==0||t->kind==3)&&(t2->kind==0||t2->kind==3)&&t->u.basic==t2->u.basic)
								return t;
			else
			{
				printf("Error type 7 at line %d: Operands type mismatched!!\n",child->line);
				return NULL;
			}

		}
		else if(strcmp(child2->name,"OR")==0)
		{
			//new temp
			Operand lb1=malloc(sizeof(struct Operand_));
			lb1->kind=LABEL;
			lb1->u.label_no=labCount++;

			child2=child2->brother;
			Type t=Exp_Cond(child,label_true,lb1);	//code1

			InterCode lb1code=malloc(sizeof(struct InterCode));
			lb1code->kind=LABEL_K;
			lb1code->u.one.op=lb1;
			insertCode(lb1code);		//label 1

			Type t2=Exp_Cond(child2,label_true,label_false);	//code2
			if(t==NULL||t2==NULL)return NULL;
			else if((t->kind==0||t->kind==3)&&(t2->kind==0||t2->kind==3)&&t->u.basic==t2->u.basic)
				return t;
			else
			{
				printf("Error type 7 at line %d: Operands type mismatched!!!\n",child->line);
				return NULL;
			}

		}

	}
	if(strcmp(child->name,"NOT")==0)	//not
	{
		child=child->brother;
		Type t=Exp_Cond(child,label_false,label_true);
		if(t==NULL)return NULL;
		if(t->kind==0&&t->u.basic==INTTYPE)return t;
		printf("Error type 7 at line %d: Operands type mismatched\n",child->line);
		return NULL;
	}
	Operand t1=malloc(sizeof(struct Operand_));
	t1->kind=TEMPVAR;
	t1->u.tmp_no=tempcount++;
	type=Exp(n,t1);		//code1
	InterCode code2=malloc(sizeof(struct InterCode));
	code2->kind=IFGOTO_K;
	code2->u.triop.t1=t1;
	code2->u.triop.op=malloc(2);
	strcpy(code2->u.triop.op,"!=");
	Operand t2=malloc(sizeof(struct Operand_));
	t2->kind=CONSTANT;
	t2->u.value=malloc(32);
	strcpy(t2->u.value,"0");
	code2->u.triop.t2=t2;
	code2->u.triop.label=label_true;
	insertCode(code2);		//code2

	InterCode gotolbf=malloc(sizeof(struct InterCode));
	gotolbf->kind=GOTO_K;
	gotolbf->u.one.op=label_false;
	insertCode(gotolbf);		//goto label false
	return type;
}

bool Args(tree* n,FieldList f,Operand arg_list)
{//printName(n->name);
	if(n==NULL&&f==NULL) return true;
	else if(n==NULL||f==NULL) return false;
	Operand t1=malloc(sizeof(struct Operand_));
	t1->kind=TEMPVAR;
	t1->u.tmp_no=tempcount++;
	tree *child=n->children;
	Type t=Exp(child,t1);			//code1
	t1->next=arg_list->next;
	arg_list->next=t1;
	if(t==NULL)return true;		//don't need to report the mistake again
	if(!typeEqual(t,f->type))return false;
	if(child->brother==NULL&&f->tail==NULL)return true;
	else if(child->brother==NULL||f->tail==NULL)return false;
	return Args(child->brother->brother,f->tail,arg_list);	//code2
}
//some useful function
void printparam(FieldList f)
{
	while(f!=NULL)
	{
		printtype(f->type);
		f=f->tail;
	}
}

void printargs(tree *n)
{
	tree *child=n->children;
	Type t=Exp(child,NULL);
	if(t==NULL)return;
	printtype(t);
	child=child->brother;
	if(child==NULL)return;
	child=child->brother;
	printargs(child);
}

void printtype(Type t){
	if((t->kind==0||t->kind==3)&&t->u.basic==INTTYPE)
		printf(" int ");
	else if((t->kind==0||t->kind==3)&&t->u.basic==FLOATTYPE)
		printf(" float ");
	else if(t->kind==2)
		printf("struct %s ",t->u.structure->name);
	else if(t->kind==1){
		printtype(t->u.array.elem);
		printf("[]");
	}
}

void printNode(tree *n)
{
	tree *child=n->children;
	if(child==NULL)
	{
		printf(" %s",n->value);
		return;
	}
	while(child!=NULL)
	{
		printNode(child);
		child=child->brother;
	}
}
int typeSize(Type type)
{
	if(type->kind==0||type->kind==3)
	{
		if(type->u.basic==INTTYPE)
			return 4;
		else return 8;
	}
	else if(type->kind==2)	//struct
	{
		int size=0;
		FieldList f=type->u.structure->inList;
		while(f!=NULL)
		{
			size+=typeSize(f->type);
			f=f->tail;
		}
		return size;
	}
	else if(type->kind==1)		//array
	{
		//高维数组
		if(type->u.array.elem->kind==1)
		{
			printf("Can not translate the code: Contain multidimensional array and function parameters of array type!\n");
			exit(-1);
		}
		return	type->u.array.size*typeSize(type->u.array.elem);
	}
	printf("type size error!\n");
	return 0;
}
