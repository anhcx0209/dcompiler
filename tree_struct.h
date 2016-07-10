#ifndef TREE_STRUCT_H
#define TREE_STRUCT_H

#include <malloc.h>
#include <string.h>

enum Direction {
	_LEFT = 0, _RIGHT = 1
};

enum ExprType {
	_IDVAL = 0, 
	_FVAL = 1, 
	_IVAL = 2,
	_CHVAL = 3,
	_SVAL = 4,
	_FUNCALL = 5,
	_ASS = 6,
	_ADD_ASS = 7,
	_SUB_ASS = 8, 
	_MUL_ASS = 9, 
	_DIV_ASS = 10,
	_OR_OR = 11, 
	_AND_AND = 12, 
	_EQ = 13, 
	_NEQ = 14, 
	_GT = 15, 
	_LT = 16, 
	_GE = 17, 
	_LE = 18,
	_ADD = 19, 
	_SUB = 20, 
	_MUL = 21, 
	_DIV = 22,	
	_ADD_ADD_B = 23, 
	_SUB_SUB_B = 24,	
	_NOT = 25, 
	_U_MINUS = 26, 
	_POSTFIX = 27,
	_CAT_ASS = 28,
	_CAT = 29,
	_CASTING_TYPE = 30,	
	_POSTFIX_ASS = 31,
	_READLN = 32,
	_WRITELN = 33, 
	_ADD_ADD_A = 34,
	_SUB_SUB_A = 35
};

enum BasicType {
	_UNKNOWN = 0, 
	_INT = 1, 
	_FLOAT = 2,
	_CHAR = 3,
	_CLASS = 4,
	_VOID = 5,
	_STRING = 6,
	_ARRAY = 7
};

enum StmType {
	_EXPR_STM = 0,
	_LIB_STM = 1,
	_VAR_DECL_STM = 2,
	_FUNC_DECL_STM = 3,
	_FUNC_DEF_STM = 4, 
	_IF_STM = 5,
	_DO_STM = 6,
	_WHILE_STM = 7,
	_SWITCH_STM = 8,
	_CASE_STM = 9,
	_BREAK_STM = 10,
	_CONTINUE_STM = 11,
	_FOR_STM = 12,
	_DEFAULT_STM = 13,
	_RETURN_STM = 14
};

struct SemanticType {
	enum BasicType type;
	enum BasicType basic_type;
	int dim_count;
};

struct Expression {
	int ival;
	float fval;
	char cval;
	char * sval;		

	enum ExprType type;
	struct Expression *left;
	struct Expression *right;
	struct Expression *mid;

	// for semantic checking
	struct SemanticType semantic_type;
		
	// only have if expression is arguments of function
	struct Expression *next;
	int next_expr_count;
	
	int line;
};

struct Program {	
	char *name;
	struct StatementList *stm_list;
};

struct StatementList {
	struct Statement *first;
	struct Statement *last;
	int size;
};

struct DotId {	
	char *name_id;	
	struct DotId *next;
};

struct Dimention {
	int size;	
	struct Dimention *next;
};

struct Variable {
	char *name;
	enum BasicType type;
	struct Dimention *dim;
	struct Expression *init_expr;
	
	struct Variable *next;

	struct SemanticType semantic_type;
	int line;
};

struct VariableDecl {
	int is_constant;
	enum BasicType type;
	struct Dimention *dim;

	struct Variable *var_list;	
};

struct FunctionDecl {
	char *name;
	enum BasicType ret_type;
	struct Dimention *ret_dim;
	struct Variable *arg_list;
};

struct FunctionDef {
	struct FunctionDecl *header;
	struct StatementList *body;
};

struct IfStm {
	struct Expression *condition;
	struct StatementList *thenclause;
	struct StatementList *elseclause;
};

struct DoStm {
	struct Expression *condition;
	struct StatementList *body;
};

struct WhileStm {
	struct Expression *condition;
	struct StatementList *body;
};

struct ForStm {
	struct Variable * variable;
	struct Expression *expr1;
	struct Expression *expr2;
	struct StatementList *body;
};

struct SwitchStm {
	struct Expression *condition;
	struct StatementList *body;
};

struct CaseStm {
	struct Expression * option;
	struct StatementList *body;
};

struct Statement {
	enum StmType type;					
	char *name;							

	struct DotId *lib_name;				
	struct VariableDecl *var_decl;		
	struct FunctionDecl *fun_decl;		
	struct FunctionDef *fun_def;		

	struct Expression *expr_stm;		
	struct IfStm *if_stm;				
	struct DoStm *do_stm;				
	struct WhileStm *while_stm;			
	struct ForStm *for_stm;				
	struct SwitchStm *switch_stm;		
	struct CaseStm *case_stm;			
	struct StatementList *default_stm;	

	struct Statement *next;
};

#endif