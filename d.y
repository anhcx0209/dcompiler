%{	
	#include "tree_struct.h"
	#include <stdio.h>
	#include <string.h>
	#include <malloc.h>

	void yyerror(char const *s);
	extern int yylex(void);
	extern int line_number;

	// PROGRAM FUNCTIONS
	struct Program *CreateProgram(struct StatementList *, struct StatementList *);

	// STATEMENT LIST FUNCTIONS
	struct StatementList *AddStmToStmList(struct StatementList *, struct Statement *);
	
	// EXPRESSION
	struct Expression *CreateExpr(int, void *, void *);
	struct Expression *CreateExprFloat(float);
	struct Expression *AddArgToArgList(struct Expression *, struct Expression *);

	// STATEMENT FUNCTIONS
	struct Statement *CreateStm(int);
	struct Statement *CreateLibDecStm(struct DotId *);
	struct Statement *CreateStmFromVarDecl(struct VariableDecl *);
	struct Statement *CreateStmFromFunDecl(struct FunctionDecl *);
	struct Statement *CreateStmFromFunDef(struct FunctionDef *);

	struct Statement *CreateStmFromExpr(struct Expression *);
	struct Statement *CreateStmFromIfStm(struct Expression *, struct StatementList *, struct StatementList *);
	struct Statement *CreateStmFromDoStm(struct Expression *, struct StatementList *);
	struct Statement *CreateStmFromWhileStm(struct Expression *, struct StatementList *);
	struct Statement *CreateStmFromReturnStm(struct Expression *);


	struct Statement *CreateStmFromSwitchStm(struct Expression *, struct StatementList *, struct Statement *);
	struct Statement *CreateStmFromCaseStm(struct Expression *, struct StatementList *);
	struct Statement *CreateStmFromDefaultStm(struct StatementList *);
	struct Statement *CreateStmFromBreakContinueStm(int);
	struct Statement *CreateStmFromForStm(int, struct Variable *, struct Expression *, struct Expression *, struct StatementList *);

	// FUNCTION DEFINATIONS
	struct FunctionDef *CreateFunDef(int, struct Dimention *, char *, struct Variable *, struct StatementList *);	

	// FUNCTION DECLARATION
	struct FunctionDecl *CreateFunDecl(int, struct Dimention *, char *, struct Variable *);	

	// VARIABLE DECLARATION FUNCTIONS 
	struct VariableDecl *CreateVarDec(int, struct Dimention *, struct Variable *, int);

	// VARIABLE 
	struct Variable *CreateVar(int, char *, struct Dimention *, struct Expression *);	
	struct Variable *AddVar(struct Variable *, struct Variable *);

	// LIST ID WITH DOT FUNCTIONS	
	struct DotId *AddIdToDotId(struct DotId *, char *);

	// DIMENTION FUNCTIONS
	struct Dimention *AddDimToDimList(struct Dimention *, int);

	// PROGRAM
	struct Program *prog;
%}

%union {	
	int ival;
	float fval;
	char *strval;		
	char cval;	
	
	struct Expression *exp_type;
	struct Program *pro_type;
	struct StatementList *stmlist_type;
	struct Statement *stm_type;
	struct DotId *dot_id_type;
	struct Dimention *dim_type;
	struct Variable *var_type;
	struct VariableDecl *var_decl_type;
	struct FunctionDecl *fun_decl_type;
	struct FunctionDef *fun_def_type;
	struct FunDeclArg *fun_decl_arg_type;
	struct Expression *expr_type;	
	struct ArgumentList *arg_fun_call_type;
	enum BasicType b_type;
}

%type <pro_type>Program
%type <stmlist_type>GlobalStatementList LibraryDeclarations LocalStatementList CaseStatements ComposeStatement ScopeStatement
%type <stm_type>LibraryDeclaration GlobalStatement LocalStatement CaseStatement DefaultStatement
%type <var_decl_type>VariableDeclaration
%type <fun_decl_type>FunctionDeclaration
%type <fun_def_type> FunctionDefination
%type <dot_id_type>DotID
%type <dim_type>ArrayType 
%type <var_type>Declarator Declarators PrototypeParameters PrototypeParameter Parameter Parameters
%type <b_type> BasicType
%type <expr_type>Expression Arguments

%token <ival> INTVAL
%token <fval> FLOATVAL
%token <strval> STRINGVAL ID
%token <cval> CHARVAL

%token BREAK CONTINUE DEFAULT RETURN IF WHILE DO CASE SWITCH FOR IMPORT CONST INT FLOAT CHAR VOID ADD_ADD SUB_SUB READLN WRITELN

%left ','
%left '=' ADD_ASS SUB_ASS MUL_ASS DIV_ASS CAT_ASS
%left OR_OR
%left AND_AND
%left EQ NEQ '<' LE '>' GE
%left '+' '-' '~'
%left '*' '/'
%left ADDADDB SUBSUBB UMINUS '!'
%left ADDADDA SUBSUBA
%right ADD_ADD SUB_SUB
%left '[' ']'
%left '(' ')'
%right ELSE CASE DEFAULT

%start Program
%%

Program
	: GlobalStatementList {$$=prog=CreateProgram($1, NULL);}
	| LibraryDeclarations {$$=prog=CreateProgram($1, NULL);}
	| LibraryDeclarations GlobalStatementList {$$=prog=CreateProgram($1, $2);}
	;
	
LibraryDeclarations	
	: LibraryDeclaration {$$=AddStmToStmList(NULL, $1);}
	| LibraryDeclarations LibraryDeclaration {$$=AddStmToStmList($1, $2);}
	;
	
LibraryDeclaration
	: IMPORT DotID ';' {$$=CreateLibDecStm($2);}
	;

GlobalStatementList
	: GlobalStatement {$$=AddStmToStmList(NULL, $1);}
	| GlobalStatementList GlobalStatement {$$=AddStmToStmList($1, $2);}
	;

GlobalStatement
	: VariableDeclaration ';' {$$=CreateStmFromVarDecl($1);}
	| FunctionDeclaration ';' {$$=CreateStmFromFunDecl($1);}
	| FunctionDefination {$$=CreateStmFromFunDef($1);}
	;

FunctionDefination
	: VOID ID '(' Parameters ')' ComposeStatement {$$=CreateFunDef(_VOID, NULL, $2, $4, $6);}
	| VOID ID '(' ')' ComposeStatement {$$=CreateFunDef(_VOID, NULL, $2, NULL, $5);}	
	| BasicType ID '(' ')' ComposeStatement {$$=CreateFunDef($1, NULL, $2, NULL, $5);}
	| BasicType ID '(' Parameters ')' ComposeStatement {$$=CreateFunDef($1, NULL, $2, $4, $6);}
	| BasicType ArrayType ID '(' ')' ComposeStatement {$$=CreateFunDef($1, $2, $3, NULL, $6);}
	| BasicType ArrayType ID '(' Parameters ')' ComposeStatement {$$=CreateFunDef($1, $2, $3, $5, $7);}
	;

ComposeStatement
	: '{' '}' {$$=AddStmToStmList(NULL, NULL);}
	| '{' LocalStatementList '}' {$$=$2;}
	;

LocalStatementList
	: LocalStatement {$$=AddStmToStmList(NULL, $1);}
	| LocalStatementList LocalStatement {$$=AddStmToStmList($1, $2);}
	; 

LocalStatement
	: ';' {$$=NULL;}
	| Expression ';' {$$=CreateStmFromExpr($1);}
	| VariableDeclaration ';' {$$=CreateStmFromVarDecl($1);}
	| IF '(' Expression ')' ScopeStatement {$$=CreateStmFromIfStm($3, $5, NULL);}
	| IF '(' Expression ')' ScopeStatement ELSE ScopeStatement {$$=CreateStmFromIfStm($3, $5, $7);}
	| DO ScopeStatement WHILE '(' Expression ')' ';' {$$=CreateStmFromDoStm($5, $2);}
	| WHILE '(' Expression ')' ScopeStatement {$$=CreateStmFromWhileStm($3, $5);}
	| SWITCH '(' Expression ')' '{' CaseStatements '}' {$$=CreateStmFromSwitchStm($3, $6, NULL);}
	| SWITCH '(' Expression ')' '{' CaseStatements DefaultStatement '}' {$$=CreateStmFromSwitchStm($3, $6, $7);}
	| BREAK ';' {$$=CreateStmFromBreakContinueStm(_BREAK_STM);}
	| CONTINUE ';' {$$=CreateStmFromBreakContinueStm(_CONTINUE_STM);}
	| FOR '(' BasicType Declarator ';' Expression ';' Expression ')' ScopeStatement	{$$=CreateStmFromForStm($3, $4, $6, $8, $10);}
	| FOR '(' Declarator ';' Expression ';' Expression ')' ScopeStatement	{$$=CreateStmFromForStm(_UNKNOWN, $3, $5, $7, $9);}	
	| RETURN Expression ';' {$$=CreateStmFromReturnStm($2);}
	;

CaseStatements
	: CaseStatement {$$=AddStmToStmList(NULL, $1);}
	| CaseStatements CaseStatement {$$=AddStmToStmList($1, $2);}
	;

CaseStatement
	: CASE Expression ':' LocalStatementList {$$=CreateStmFromCaseStm($2, $4);}
	| CASE Expression ':' ComposeStatement {$$=CreateStmFromCaseStm($2, $4);}
	;

DefaultStatement
	: DEFAULT ':' LocalStatementList {$$=CreateStmFromDefaultStm($3);}
	| DEFAULT ':' ComposeStatement {$$=CreateStmFromDefaultStm($3);}
	;

ScopeStatement
	: LocalStatement {$$=AddStmToStmList(NULL, $1);}
	| ComposeStatement {$$=$1;}
	;

Arguments
	: Expression {$$=AddArgToArgList(NULL, $1);}
	| Arguments ',' Expression {$$=AddArgToArgList($1, $3);}
	;

Expression
	: INTVAL {$$=CreateExpr(_IVAL, (void *)(&($1)), NULL); }
	| FLOATVAL {$$=CreateExprFloat($1);}
	| CHARVAL {$$=CreateExpr(_CHVAL, (void *)(&($1)), NULL);}
	| STRINGVAL {$$=CreateExpr(_SVAL, (void *)($1), NULL);}
	| ID {$$=CreateExpr(_IDVAL, (void *)($1), NULL);}
	| Expression '=' Expression {$$=CreateExpr(_ASS, (void *)$1, (void *)$3);}
	| Expression '+' Expression {$$=CreateExpr(_ADD, (void *)$1, (void *)$3);}
	| Expression '-' Expression {$$=CreateExpr(_SUB, (void *)$1, (void *)$3);}
	| Expression '*' Expression {$$=CreateExpr(_MUL, (void *)$1, (void *)$3);}	
	| Expression '/' Expression {$$=CreateExpr(_DIV, (void *)$1, (void *)$3);}	
	| Expression '~' Expression {$$=CreateExpr(_CAT, (void *)$1, (void *)$3);}
	| Expression ADD_ASS Expression {$$=CreateExpr(_ADD_ASS, (void *)$1, (void *)$3);}
	| Expression SUB_ASS Expression {$$=CreateExpr(_SUB_ASS, (void *)$1, (void *)$3);}
	| Expression MUL_ASS Expression {$$=CreateExpr(_MUL_ASS, (void *)$1, (void *)$3);}
	| Expression DIV_ASS Expression {$$=CreateExpr(_DIV_ASS, (void *)$1, (void *)$3);}
	| Expression CAT_ASS Expression {$$=CreateExpr(_CAT_ASS, (void *)$1, (void *)$3);}
	| Expression AND_AND Expression {$$=CreateExpr(_AND_AND, (void *)$1, (void *)$3);}
	| Expression OR_OR Expression {$$=CreateExpr(_OR_OR, (void *)$1, (void *)$3);}
	| Expression EQ Expression {$$=CreateExpr(_EQ, (void *)$1, (void *)$3);}
	| Expression NEQ Expression {$$=CreateExpr(_NEQ, (void *)$1, (void *)$3);}
	| Expression '>' Expression {$$=CreateExpr(_GT, (void *)$1, (void *)$3);}
	| Expression '<' Expression {$$=CreateExpr(_LT, (void *)$1, (void *)$3);}	
	| Expression GE Expression {$$=CreateExpr(_GE, (void *)$1, (void *)$3);}
	| Expression LE Expression {$$=CreateExpr(_LE, (void *)$1, (void *)$3);}
	| '!' Expression {$$=CreateExpr(_NOT, (void *)$2, NULL);}	
	| '-' Expression %prec UMINUS {$$=CreateExpr(_U_MINUS, (void *)$2, NULL);}
	| ADD_ADD Expression %prec ADDADDB {$$=CreateExpr(_ADD_ADD_B, (void *)$2, NULL);}
	| SUB_SUB Expression %prec SUBSUBB {$$=CreateExpr(_SUB_SUB_B, (void *)$2, NULL);}		
	| Expression ADD_ADD %prec ADDADDA {$$=CreateExpr(_ADD_ADD_A, (void *)$1, NULL);}
	| Expression SUB_SUB %prec SUBSUBA {$$=CreateExpr(_SUB_SUB_A, (void *)$1, NULL);}
	| '(' Expression ')' {$$=$2;}
	| ID '(' ')' {$$=CreateExpr(_FUNCALL, (void *)$1, NULL);}
	| ID '(' Arguments ')' {$$=CreateExpr(_FUNCALL, (void *)$1, (void *)$3);}
	| Expression '[' Expression ']' {$$=CreateExpr(_POSTFIX, (void *)$1, (void *)$3);}
	| WRITELN '(' Expression ')' {$$=CreateExpr(_WRITELN, (void *)$3, NULL);}
	| READLN '(' Expression ')' {$$=CreateExpr(_READLN, (void *)$3, NULL);}
	;

FunctionDeclaration
	: VOID ID '(' PrototypeParameters  ')' {$$=CreateFunDecl(_VOID, NULL, $2, $4);}
	| VOID ID '(' Parameters  ')' {$$=CreateFunDecl(_VOID, NULL, $2, $4);}
	| VOID ID '(' ')' {$$=CreateFunDecl(_VOID, NULL, $2, NULL);}
	| BasicType ID '(' PrototypeParameters  ')' {$$=CreateFunDecl($1, NULL, $2, $4);}
	| BasicType ID '(' Parameters  ')' {$$=CreateFunDecl($1, NULL, $2, $4);}
	| BasicType ID '(' ')' {$$=CreateFunDecl($1, NULL, $2, NULL);}
	| BasicType ArrayType ID '(' ')' {$$=CreateFunDecl($1, $2, $3, NULL);}
	| BasicType ArrayType ID '(' PrototypeParameters  ')' {$$=CreateFunDecl($1, $2, $3, $5);}
	| BasicType ArrayType ID '(' Parameters  ')' {$$=CreateFunDecl($1, $2, $3, $5);}
	;

PrototypeParameters
	: PrototypeParameter {$$=$1;}
	| PrototypeParameters ',' PrototypeParameter {$$=AddVar($1, $3);}
	;	

Parameters
	: Parameter {$$=$1;}
	| Parameters ',' Parameter {$$=AddVar($1, $3);}
	;

Parameter
	: BasicType ArrayType ID {$$=CreateVar($1, $3, $2, NULL);}
	| BasicType ID {$$=CreateVar($1, $2, NULL, NULL);}	
	;

PrototypeParameter
	: BasicType {$$=CreateVar($1, NULL, NULL, NULL);}
	| BasicType ArrayType {$$=CreateVar($1, NULL, $2, NULL);}		
	;

VariableDeclaration
	: CONST BasicType Declarators {$$=CreateVarDec($2, NULL, $3, 1);}
	| BasicType Declarators {$$=CreateVarDec($1, NULL, $2, 0);}
	| BasicType ArrayType Declarators {$$=CreateVarDec($1, $2, $3, 0);}
	| CONST BasicType ArrayType Declarators {$$=CreateVarDec($2, $3, $4, 1);}
	;
	
BasicType
	: INT {$$=_INT;}
	| FLOAT {$$=_FLOAT;}
	| CHAR {$$=_CHAR;}	
	;
	
ArrayType
	: '[' INTVAL ']' {$$=AddDimToDimList(NULL, $2);}	
	| ArrayType '[' INTVAL ']' {$$=AddDimToDimList($1, $3);}
	;

Declarators 
	: Declarator {$$=$1;}
	| Declarators ',' Declarator {$$=AddVar($1, $3);}
	;

Declarator
	: ID {$$=CreateVar(_UNKNOWN, $1, NULL, NULL);}
	| ID '=' Expression {$$=CreateVar(_UNKNOWN, $1, NULL, $3);}
	;
	
DotID
	: ID {$$=AddIdToDotId(NULL, $1);}
	| DotID '.' ID {$$=AddIdToDotId($1, $3);}
	;	
	
%%

void yyerror(char const *s)
{
	printf("Line %d: %s\n", line_number, s);	
	exit(-1);
}

struct Program *CreateProgram(struct StatementList *list1, struct StatementList *list2) {
	struct Program *program = (struct Program *)malloc(sizeof(struct Program));	
	program->name = (char *)malloc(strlen("DLangCode") * sizeof(char));
	strcpy(program->name, "DLangCode");

	struct StatementList *stmlist = list1;
	if (list2 != NULL) {
		stmlist->last->next = list2->first;
		stmlist->last = list2->last;
	}		

	program->stm_list = stmlist;
	return program;
}

struct StatementList *AddStmToStmList(struct StatementList *stmlist, struct Statement *stm) {
	if (stmlist == NULL) {		
		stmlist = (struct StatementList *)malloc(sizeof(struct StatementList));		
		stmlist->first = stmlist->last = stm;		 
	} else {
		stmlist->last->next = stm; // let last node point to new node
		stmlist->last = stm;	// change last node	
	}
	return stmlist;
}

struct Statement *CreateLibDecStm(struct DotId *dot_id) {
	struct Statement *stm = (struct Statement *)malloc(sizeof(struct Statement));
	stm->name = (char *)malloc(strlen("Import"));
	strcpy(stm->name, "Import");
	stm->type = _LIB_STM;	
	stm->lib_name = dot_id;
	stm->next = NULL;
	
	return stm;
}

struct DotId *AddIdToDotId(struct DotId *dot_id, char *name) {
	struct DotId *dot_name = (struct DotId *)malloc(sizeof(struct DotId));		
	dot_name->name_id = name;
	dot_name->next = NULL;		

	if (dot_id == NULL) {
		return dot_name;
	} else {
		struct DotId *d = dot_id;
		while (d->next != NULL) d = d->next;
		d->next = dot_name;
		return dot_id;
	}
}

struct Variable *CreateVar(int type, char *name, struct Dimention *dim, struct Expression *expr) {
	struct Variable *var = (struct Variable *)malloc(sizeof(struct Variable));	
	var->type = (enum BasicType)type;
	var->name = name;
	var->dim = (dim != NULL) ? dim : NULL;
	var->init_expr = (expr != NULL) ? expr : NULL;	
	var->next = NULL;
	var->line = line_number;
	return var;
}

struct Variable *AddVar(struct Variable *list, struct Variable *node) {
	struct Variable * v = list;
	while (v->next != NULL) v = v->next;	
	v->next = node;
	return list;
}

struct Dimention *AddDimToDimList(struct Dimention *list, int i) {
	struct Dimention *dim = (struct Dimention *)malloc(sizeof(struct Dimention));
	dim->size = i;
	dim->next = NULL;	

	if (list == NULL) {	
		return dim;
	} else {
		struct Dimention * d = list;
		while (d->next != NULL) d = d->next;
		d->next = dim;		
		return list;
	}	
}

struct VariableDecl *CreateVarDec(int t, struct Dimention * d, struct Variable *vars, int isConst) {
	struct VariableDecl  *var_decl = (struct VariableDecl *)malloc(sizeof(struct VariableDecl));
	var_decl->is_constant = isConst;
	var_decl->type = (enum BasicType)t;
	var_decl->dim = d;
	var_decl->var_list = vars;

	// apply dim and basic type for all variable in one declaration
	for (struct Variable *i = var_decl->var_list; i != NULL; i = i->next) {
		i->type = (BasicType)t;
		i->dim = d;
	}
	
	return var_decl;
}

struct Statement *CreateStmFromVarDecl(struct VariableDecl * vd) {
	struct Statement *stm = (struct Statement *)malloc(sizeof(struct Statement));
	stm->name = (char *)malloc(15 * sizeof(char));
	strcpy(stm->name, "Var Decl");
	stm->type = _VAR_DECL_STM;
	stm->var_decl = vd;
	stm->next = NULL;

	return stm;
}

struct FunctionDecl *CreateFunDecl(int rtype, struct Dimention * rdim, char * fname, struct Variable * vars) {
	struct FunctionDecl *func_decl = (struct FunctionDecl *)malloc(sizeof(struct FunctionDecl));
	func_decl->ret_type = (enum BasicType)rtype;
	func_decl->ret_dim = rdim;
	func_decl->name = fname;
	func_decl->arg_list = vars;

	return func_decl;
}

struct Statement *CreateStmFromFunDecl(struct FunctionDecl * funDecl) {
	struct Statement *stm = (struct Statement *)malloc(sizeof(struct Statement));
	stm->name = (char *)malloc(15 * sizeof(char));
	strcpy(stm->name, "Func Decl");
	stm->type = _FUNC_DECL_STM;
	stm->fun_decl = funDecl;
	stm->next = NULL;
	
	return stm;
}

struct FunctionDef *CreateFunDef(int rtype, struct Dimention *rdim, char *fname, struct Variable *params, struct StatementList *body) {
	struct FunctionDef *func_def = (struct FunctionDef *)malloc(sizeof(struct FunctionDef));
	struct FunctionDecl *header = CreateFunDecl(rtype, rdim, fname, params);
	func_def->header = header;
	func_def->body = body;
	return func_def;
}

struct Statement *CreateStmFromFunDef(struct FunctionDef * funDef) {
	struct Statement *stm = (struct Statement *)malloc(sizeof(struct Statement));
	stm->name = (char *)malloc(15 * sizeof(char));
	strcpy(stm->name, "Func Def");
	stm->type = _FUNC_DEF_STM;
	stm->fun_def = funDef;
	stm->next = NULL;
	
	return stm;
}

struct Expression *CreateExprFloat(float f) {
	struct Expression *expr = (struct Expression *)malloc(sizeof(struct Expression));
	expr->type = _FVAL;
	expr->left = expr->right = expr->next = expr->mid = NULL;
	expr->fval = f;
	expr->line = line_number;

	return expr;
}

struct Expression *CreateExpr(int type, void *left, void *right) {
	struct Expression *expr = (struct Expression *)malloc(sizeof(struct Expression));
	expr->type = (enum ExprType)type;
	expr->right = expr->left = expr->mid = expr->next = NULL;

	switch (type) {
		case _IDVAL:
			expr->sval = (char *)left;
			break;
		case _FVAL:
			expr->fval = *((float *)left);			
			break;	
		case _IVAL:
			expr->ival = *((int *)left);			
			break;	
		case _CHVAL:
			expr->cval = *((char *)left);
			break;
		case _SVAL:
			expr->sval = (char *)left;
			break;
		case _FUNCALL:
			expr->sval = (char *)left;
			expr->left = (struct Expression *)right;
			break;
		case _POSTFIX:
			expr->left = (struct Expression *)left;
			expr->right = (struct Expression *)right;
			break;	
		case _ADD_ADD_B:
		case _SUB_SUB_B:
		case _NOT:
		case _U_MINUS:			
		case _WRITELN:
		case _READLN:
			expr->left = (struct Expression *)left;
			break;
		default:
			expr->left = (struct Expression *)left;
			expr->right = (struct Expression *)right;
			break;	
	}	

	expr->line = line_number;
	return expr;
}

struct Expression *AddArgToArgList(struct Expression *root, struct Expression *newNode) {				
	if (root == NULL) {		
		struct Expression *ret = (struct Expression *)malloc(sizeof(struct Expression));
		ret = newNode;
		return ret;
	} else {						
		struct Expression * iter = root;		
		while (iter->next != NULL) {
			iter = iter->next;			
		}		
		iter->next = newNode;	
		return root;
	}	
}

struct Statement *CreateStmFromExpr(struct Expression * expr) {
	struct Statement *stm = (struct Statement *)malloc(sizeof(struct Statement));
	stm->name = (char *)malloc(15 * sizeof(char));
	strcpy(stm->name, "Expr");
	stm->type = _EXPR_STM;
	stm->expr_stm = expr;
	stm->next = NULL;

	return stm;
}

struct Statement *CreateStmFromIfStm(struct Expression *cond, struct StatementList *thenstm, struct StatementList *elsestm) {
	struct Statement *ret = (struct Statement *)malloc(sizeof(struct Statement));
	ret->name = (char *)malloc(15 * sizeof(char));
	strcpy(ret->name, "If");
	ret->type = _IF_STM;

	// Create if stm and condition
	struct IfStm *ifstm = (struct IfStm *)malloc(sizeof(struct IfStm));
	ifstm->condition = cond;
	ifstm->thenclause = thenstm;
	ifstm->elseclause = elsestm;

	ret->if_stm = ifstm;
	ret->next = NULL;
	
	return ret;
}

struct Statement *CreateStmFromDoStm(struct Expression * cond, struct StatementList * stm) {
	struct Statement *ret = (struct Statement *)malloc(sizeof(struct Statement));
	ret->name = (char *)malloc(15 * sizeof(char));
	strcpy(ret->name, "Do");
	ret->type = _DO_STM;

	// Create do stm and condition
	struct DoStm *dostm = (struct DoStm *)malloc(sizeof(struct DoStm));
	dostm->condition = cond;
	dostm->body = stm;
	ret->do_stm = dostm;
	ret->next = NULL;
	
	return ret;
}

struct Statement *CreateStmFromWhileStm(struct Expression * cond, struct StatementList * stm) {
	struct Statement *ret = (struct Statement *)malloc(sizeof(struct Statement));
	ret->name = (char *)malloc(15 * sizeof(char));
	strcpy(ret->name, "While");
	ret->type = _WHILE_STM;

	// Create do stm and condition
	struct WhileStm *whilestm = (struct WhileStm *)malloc(sizeof(struct WhileStm));
	whilestm->condition = cond;
	whilestm->body = stm;
	ret->while_stm = whilestm;
	ret->next = NULL;
	
	return ret;
}

struct Statement *CreateStmFromSwitchStm(struct Expression *cond, struct StatementList *stmlist, struct Statement *stm) {
	struct Statement *ret = (struct Statement *)malloc(sizeof(struct Statement));
	ret->name = (char *)malloc(15 * sizeof(char));
	strcpy(ret->name, "Switch");
	ret->type = _SWITCH_STM;

	// Create do stm and condition
	struct SwitchStm *switchstm = (struct SwitchStm *)malloc(sizeof(struct SwitchStm));
	switchstm->condition = cond;
	switchstm->body = stmlist;
	stmlist->last->next = stm;
	stmlist->last = stm;
	ret->switch_stm = switchstm;
	ret->next = NULL;
	
	return ret;
}

struct Statement *CreateStmFromCaseStm(struct Expression * expr, struct StatementList * stm) {
	struct Statement *ret = (struct Statement *)malloc(sizeof(struct Statement));
	ret->name = (char *)malloc(15 * sizeof(char));
	strcpy(ret->name, "Case");
	ret->type = _CASE_STM;

	// Create do stm and condition
	struct CaseStm *casestm = (struct CaseStm *)malloc(sizeof(struct CaseStm));
	casestm->option = expr;
	casestm->body = stm;	

	ret->case_stm = casestm;
	ret->next = NULL;
	return ret;
}

struct Statement *CreateStmFromDefaultStm(struct StatementList * body) {
	struct Statement *ret = (struct Statement *)malloc(sizeof(struct Statement));
	ret->name = (char *)malloc(15 * sizeof(char));
	strcpy(ret->name, "Default");
	ret->type = _DEFAULT_STM;
	ret->default_stm = body;
	ret->next = NULL;
	
	return ret;
}

struct Statement *CreateStmFromBreakContinueStm(int type) {
	struct Statement *ret = (struct Statement *)malloc(sizeof(struct Statement));
	ret->name = (char *)malloc(15 * sizeof(char));
	ret->type = (enum StmType)type;
	if (type == _BREAK_STM)
		strcpy(ret->name, "Break");
	else 
		strcpy(ret->name, "Continue");	
	ret->next = NULL;

	return ret;
}

struct Statement *CreateStmFromForStm(int type, struct Variable * v, struct Expression * test, struct Expression * inc, struct StatementList * body) {
	struct Statement *ret = (struct Statement *)malloc(sizeof(struct Statement));
	ret->name = (char *)malloc(15 * sizeof(char));
	strcpy(ret->name, "For");
	ret->type = _FOR_STM;

	struct ForStm *newstm = (struct ForStm *)malloc(sizeof(struct ForStm));
	v->type = (enum BasicType)type;
	newstm->variable = v;
	newstm->expr1 = test;
	newstm->expr2 = inc;	
	newstm->body = body;

	ret->for_stm = newstm;
	ret->next = NULL;

	return ret;
}

struct Statement *CreateStmFromReturnStm(struct Expression *expr) {
	struct Statement *ret = (struct Statement *)malloc(sizeof(struct Statement));
	ret->name = (char *)malloc(15 * sizeof(char));
	strcpy(ret->name, "Return");
	ret->type = _RETURN_STM;
	ret->expr_stm = expr;
	ret->next = NULL;

	return ret;
}
