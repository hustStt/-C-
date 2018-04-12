#include"tmain.h"
#include"ir.h"
#define INTTYPE 0
#define FLOATTYPE 1
typedef enum {false, true}bool;
typedef struct Type* Type;
typedef struct FieldList* FieldList;
typedef struct Structure* Structure;
typedef struct FuncList* FuncList;

struct Type
{
	enum { basic, array, structure, constant} kind;
	union
	{
		// 基本类型int float
		int basic;
		// 数组类型信息包括元素类型与数组大小构成
		struct { Type elem; int size; } array;
		// 结构体类型信息是一个链表
		Structure structure;
	} u;
};

struct Structure
{
	char* name; //结构体名称
	FieldList inList;
};

struct FieldList
{
	char* name;	// 域的名字
	Type type;	// 域的类型
	int lev2;	//深度
	int count;
	FieldList tail;	// 下一个域(函数参数)
	FieldList next;//下一个域(符号表)
};

struct FuncList
{
	char* name;      //函数名称
	bool isDefined;  //是否定义0/1
	int line;        //行号
	Type ret;        //返回
	FieldList param;  //形参
	FuncList next;//下一个域
};

void initTable();//初始化
void printTable();
int insert(FieldList f);  //向符号表中插入符号
int insertFunc(FuncList f,int state);
void insertParam(FuncList f);
FieldList findF(char* name);
FieldList findst(char* name);
FuncList findFunc(char* name);
void checkFunc();
bool paramEqual(FieldList f1,FieldList f2);

/*some traverse tree functions*/
void Program(tree *n);
void ExtDefList(tree *n);
void ExtDef(tree *n);
void ExtDecList(tree *n,Type type);

Type Specifier(tree* n);
Type StructSpecifier(tree* n);

FieldList VarDec(tree *n,Type type,int stype);
FuncList FunDec(tree *n,Type type);
FieldList VarList(tree *n);
FieldList ParamDec(tree *n);

void CompSt(tree *n,Type ret);
void StmtList(tree *n,Type ret);
void Stmt(tree *n,Type ret);

FieldList DefList(tree* n,int stype);
FieldList Def(tree* n,int stype);
FieldList DecList(tree *n,Type type,int stype);
FieldList Dec(tree *n,Type type,int stype);

Type Exp(tree *n,Operand place);
Type Exp_Cond(tree *n,Operand lb1,Operand lb2);
bool Args(tree *n,FieldList f,Operand arg_list);

void printparam(FieldList f);
void printargs(tree *n);
void printtype(Type t);
bool typeEqual(Type t1,Type y2);
void printNode(tree* n);
int typeSize(Type type);

void trExtDef(FuncList f);
