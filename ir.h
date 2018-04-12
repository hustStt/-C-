#ifndef _IR_H
#define _IR_H
typedef struct Operand_* Operand;
typedef struct InterCode* InterCode;

struct Operand_
{
	enum {
		TEMPVAR,VARIABLE,VADDRESS,CONSTANT,LABEL,FUNCTION,TADDRESS
	}kind;
	union{
		int var_no;
    int tmp_no;
		int label_no;
		char* value;	//constant
    char* funcname;
		Operand name;   //vaddress,taddress
	}u;
	Operand next;		//arg use it
};

struct InterCode
{
	enum{
		ASSIGN_K,ADD_K,SUB_K,MUL_K,DIV_K,RETURN_K,LABEL_K,GOTO_K,
		IFGOTO_K,READ_K,WRITE_K,CALL_K,ARG_K,FUNCTION_K,PARAM_K,DEC_K,
		RIGHTAT_K
	}kind;

	union{
		struct{	Operand op;}one;   //return,label,goto,read,write,arg,function
		struct{Operand left,right;}assign;	//assign rightat
		struct{Operand result,op1,op2;}binop;			//add sub mul div
		struct{Operand t1;char *op;Operand t2,label;}triop;			//if_goto
		struct{
			Operand op;
			int size;
		}dec; //dec
	}u;
	InterCode prev,next;
};

/*code node functions*/
void insertCode(InterCode c);
void deleteCode(InterCode c);
void printCode(char *fname);
void printOp(Operand op,FILE *fp);

extern InterCode codehead,codetail;
extern int varCount;
extern int tempcount;
extern int labCount;

/*optimize code*/
void optIF();
int opEqual(Operand op1,Operand op2);
void lookCon();

#endif
