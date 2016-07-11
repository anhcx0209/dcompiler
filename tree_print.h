#ifndef TREE_PRINT_H
#define TREE_PRINT_H

#include <stdio.h>
#include "tree_struct.h"

int cur_id = 0;
extern struct Program *prog;
FILE *graph_file;

char *getSemanticTypeString(struct SemanticType);
char * getTypeString(enum BasicType);
char * getDimString(struct Dimention *);

// --------------------- PRINT GRAPH --------------------------
void printEdge(int, int);
void printNode(int, const char *);

// --------------------- PRINT TREE ---------------------------
void printTree();
void printProgram(int, struct Program *);

// --------------------- PRINT EXPRESSION ---------------------
void printExpr(int, struct Expression *);

// --------------------- PRINT STATEMENT ----------------------
void printStmList(int, struct StatementList *);
void printStm(int, struct Statement *);

void printLibStm(int, struct DotId *);
void printVarDecl(int, struct VariableDecl *);
void printVar(int, struct Variable *);
void printFunDecl(int, struct FunctionDecl *);
void printFunDef(int, struct FunctionDef *);
void printForStm(int, struct ForStm *);
void printIfStm(int, struct IfStm *);
void printDoStm(int, struct DoStm *);
void printWhileStm(int, struct WhileStm *);
void printSwitchStm(int, struct SwitchStm *);
void printCaseStm(int, struct CaseStm *);

void printNode(int id, const char * label) {
	fprintf(graph_file, "node%d [label=\"%s\"];\n", id, label);
}

void printEdge(int startId, int endId) {
	fprintf(graph_file, "node%d -> node%d;\n", startId, endId);
}

void printTree() {		
	graph_file = fopen("program.dot", "w");
	fprintf(graph_file, "digraph G {");
	printProgram(cur_id, prog);
	fprintf(graph_file, "}");	
	fclose(graph_file);	
}

void printProgram(int prog_id, struct Program *prog) {			
	printNode(prog_id, prog->name);	
	printEdge(prog_id, ++cur_id);
	printNode(cur_id, "Stm List");
	printStmList(cur_id, prog->stm_list);	
}

void printStmList(int stmlist_id, struct StatementList *list) {		
	if (list != NULL) {
		struct Statement *iter = list->first;
		while (iter != NULL) {			
			int stmid = ++cur_id;
			printStm(stmid, iter);
			printEdge(stmlist_id, stmid);
			iter = iter->next;
		}
	}	
}

void printStm(int id, struct Statement *stm) {	
	printNode(id, stm->name);	
	int retid;
	switch (stm->type) {
		case _FUNC_DEF_STM:
			printFunDef(id, stm->fun_def);		
			break;
		case _VAR_DECL_STM:
			printVarDecl(id, stm->var_decl);
			break;
		case _LIB_STM:
			printLibStm(id, stm->lib_name);
			break;
		case _FUNC_DECL_STM:
			printFunDecl(id, stm->fun_decl);
			break;
		case _EXPR_STM:			
			printEdge(id, ++cur_id);		
			printExpr(cur_id, stm->expr_stm);
			break;
		case _FOR_STM:
			printForStm(id, stm->for_stm);
			break;	
		case _IF_STM:
			printIfStm(id, stm->if_stm);
			break;
		case _DO_STM:
			printDoStm(id, stm->do_stm);
			break;
		case _WHILE_STM:
			printWhileStm(id, stm->while_stm);
			break;
		case _SWITCH_STM:
			printSwitchStm(id, stm->switch_stm);
			break;
		case _RETURN_STM:
			printEdge(id, ++cur_id);
			printExpr(cur_id, stm->expr_stm);
			break;
		case _CASE_STM:
			printCaseStm(id, stm->case_stm);			
			break;
		case _DEFAULT_STM:			
			printStmList(cur_id, stm->default_stm);
			break;
		case _BREAK_STM:
		case _CONTINUE_STM:
			break;
	}
}

void printVarDecl(int id, struct VariableDecl *var_decl) {
	if (var_decl->is_constant)	{
		printNode(++cur_id, "CONST");
		printEdge(id, cur_id);
	}

	struct Variable *v = var_decl->var_list;
	while (v != NULL) {
		printEdge(id, ++cur_id);
		printVar(cur_id, v);
		v = v->next;
	}
}

char *getSemanticTypeString(struct SemanticType st) {
	if (st.dim_count == 0) {
		return getTypeString(st.type);
	} else {
		if (st.basic_type == _CHAR && st.dim_count == 1) 
			return getTypeString(_STRING);
		else
			return getTypeString(_ARRAY);
	}

	return getTypeString(_UNKNOWN);
}

void printVar(int id, struct Variable * var) {	
	char *name = (char *)malloc(100);
	strcpy(name, "");
	if (var->name != NULL) {
		strcat(name, var->name);
		strcat(name, "(");
		strcat(name, getSemanticTypeString(var->semantic_type));
		strcat(name, ")");
		printNode(id, name);		
	}

	if (var->init_expr != NULL) {
		int exprid = ++cur_id;
		printEdge(id, exprid);
		printExpr(exprid, var->init_expr);
	}
}

char* getDimString(struct Dimention * dim) {
	char * str = (char *) malloc(100);	
	strcpy(str, "");
	struct Dimention *d = dim;
	while (d != NULL) {
		strcat(str, "[");
		char number[5];
		sprintf(number, "%d", d->size);
		strcat(str, number);
		strcat(str, "]");
		d = d->next;
	}	
	return str;
}

void printLibStm(int id, struct DotId *lib_name) {
	struct DotId *iter = lib_name;
	while (iter != NULL) {
		printNode(++cur_id, iter->name_id);
		printEdge(id, cur_id);
		iter = iter->next;
	}
}

char * getSymbol(enum ExprType op) {
	char * str = (char *)malloc(4 * sizeof(char));
	switch (op) {
		case _ASS:
			strcpy(str, "=");
			break;
		case _ADD_ASS:
			strcpy(str, "+=");
			break;
		case _SUB_ASS:
			strcpy(str, "-=");
			break;
		case _MUL_ASS:
			strcpy(str, "*=");
			break;
		case _DIV_ASS:
			strcpy(str, "/=");
			break;
		case _OR_OR:
			strcpy(str, "||");
			break;
		case _AND_AND:
			strcpy(str, "&&");
			break;
		case _EQ:
			strcpy(str, "==");
			break;
		case _NEQ:
			strcpy(str, "!=");
			break;
		case _GT:
			strcpy(str, ">");
			break;
		case _LT:
			strcpy(str, "<");
			break;
		case _GE:
			strcpy(str, ">=");
			break;
		case _LE:
			strcpy(str, "<=");
			break;
		case _ADD:
			strcpy(str, "+");
			break;
		case _SUB:
		case _U_MINUS:
			strcpy(str, "-");
			break;
		case _ADD_ADD_B:
		case _ADD_ADD_A:
			strcpy(str, "++");
			break;	
		case _SUB_SUB_B:
		case _SUB_SUB_A:
			strcpy(str, "--");
			break;		
		case _MUL:
			strcpy(str, "*");
			break;
		case _DIV:
			strcpy(str, "/");
			break;
		case _NOT:
			strcpy(str, "!");
			break;
		case _POSTFIX:
			strcpy(str, "[]");
			break;
		case _CAT:
			strcpy(str, "~");	
			break;
		case _CAT_ASS:
			strcpy(str, "~=");	
			break;	
		case _POSTFIX_ASS:
			strcpy(str, "[]=");	
			break;
		default:
			break;
	}

	return str;
}

void printExpr(int id, struct Expression *expr) {		
	char stropr[50] = "";
	int leftid, rightid, arglistid, midid;
	struct Expression *iter = NULL;
	char str_print[100] = "";
	switch (expr->type)
	{
		case _IVAL:
			sprintf(str_print, "%d", expr->ival);
			strcat(str_print, "(");
			strcat(str_print, "Int");
			strcat(str_print, ")");
			printNode(id, str_print);
			break;
		case _FVAL:
			sprintf(str_print, "%.2f", expr->fval);
			strcat(str_print, "(");
			strcat(str_print, "Float");
			strcat(str_print, ")");
			printNode(id, str_print);
			break;
		case _CHVAL:
			sprintf(str_print, "%c", expr->cval);
			strcat(str_print, "(");
			strcat(str_print, "Char");
			strcat(str_print, ")");
			printNode(id, str_print);
			break;
		case _SVAL:		
			sprintf(str_print, "%s", expr->sval);
			strcat(str_print, "(");
			strcat(str_print, "String");
			strcat(str_print, ")");
			printNode(id, str_print);
			break;
		case _IDVAL:
			sprintf(str_print, "%s", expr->sval);
			strcat(str_print, "(");
			strcat(str_print, getSemanticTypeString(expr->semantic_type));
			strcat(str_print, ")");
			printNode(id, str_print);
			break;				
		case _FUNCALL:		
			printNode(id, "FUNC CALL");
			// print  name
			printEdge(id, ++cur_id);
			printNode(cur_id, expr->sval);
			// print arguments list			
			arglistid = ++cur_id;
			printEdge(id, arglistid);
			printNode(arglistid, "ARGLIST");

			iter = expr->left;
			while (iter != NULL) {
				printEdge(arglistid, ++cur_id);
				printExpr(cur_id, iter);
				iter = iter->next;
			}
			break;
		case _NOT:
		case _U_MINUS:		
		case _ADD_ADD_B:
		case _SUB_SUB_B:
			sprintf(str_print, "%s", getSymbol(expr->type));
			strcat(str_print, "(");
			strcat(str_print, getSemanticTypeString(expr->semantic_type));
			strcat(str_print, ")");
			printNode(id, str_print);

			leftid = ++cur_id;
			printEdge(id, leftid);
			printExpr(leftid, expr->left);
			break;		
		case _ADD_ADD_A:
		case _SUB_SUB_A:
			sprintf(str_print, "%s", getSymbol(expr->type));
			strcat(str_print, "(");
			strcat(str_print, getSemanticTypeString(expr->semantic_type));
			strcat(str_print, ")");
			printNode(id, str_print);

			rightid = ++cur_id;
			printEdge(id, rightid);
			printExpr(rightid, expr->right);		

			break;
		case _CASTING_TYPE:
			strcat(str_print, "(");
			strcat(str_print, getSemanticTypeString(expr->semantic_type));
			strcat(str_print, ")");
			printNode(id, str_print);

			leftid = ++cur_id;
			printEdge(id, leftid);
			printExpr(leftid, expr->left);
			break;		
		case _POSTFIX_ASS:			
			sprintf(str_print, "%s", getSymbol(expr->type));		
			strcat(str_print, "(");
			strcat(str_print, getSemanticTypeString(expr->semantic_type));
			strcat(str_print, ")");
			printNode(id, str_print);

			leftid = ++cur_id;
			printEdge(id, leftid);
			printExpr(leftid, expr->left);						

			midid = ++cur_id;
			printEdge(id, midid);
			printExpr(midid, expr->mid);


			rightid = ++cur_id;
			printEdge(id, rightid);
			printExpr(rightid, expr->right);

			break;
		default:
			sprintf(str_print, "%s", getSymbol(expr->type));
			strcat(str_print, "(");
			strcat(str_print, getSemanticTypeString(expr->semantic_type));
			strcat(str_print, ")");
			printNode(id, str_print);

			// print right 
			leftid = ++cur_id;			
			printEdge(id, leftid);
			printExpr(leftid, expr->left);
			// print left
			int rightid = ++cur_id;
			printEdge(id, rightid);
			printExpr(rightid, expr->right);
			break;	
	}
}

void printFunDecl(int id, struct FunctionDecl *fun_decl) {	
	// print name
	printNode(++cur_id, fun_decl->name);
	printEdge(id, cur_id);
	// print type and dim
	printNode(++cur_id, getTypeString(fun_decl->ret_type));
	printEdge(id, cur_id);
	if (fun_decl->ret_dim != NULL) {
		char str[100] = "";
		struct Dimention *d = fun_decl->ret_dim;
		while (d != NULL) {
			strcat(str, "[");
			char num[100] = "";
			sprintf(num, "%d", d->size);
			strcat(str, num);
			strcat(str, "]");
			d = d->next;
		}
		printNode(++cur_id, str);
		printEdge(id, cur_id);
	}
	// print arg_list
	if (fun_decl->arg_list != NULL) {
		struct Variable * v = fun_decl->arg_list;
		while (v != NULL) {
			int varid = ++cur_id;
			printVar(varid, v);
			printEdge(id, varid);
			v = v->next;
		}
	}
}

void printFunDef(int id, struct FunctionDef *fun_def) {
	// print header
	printEdge(id, ++cur_id);
	printNode(cur_id, "Header");
	printFunDecl(cur_id, fun_def->header);
	// print body
	printEdge(id, ++cur_id);
	printNode(cur_id, "Body");
	printStmList(cur_id, fun_def->body);
}

char * getTypeString(enum BasicType t) {
	char * ret = (char *)malloc(10 * sizeof(char));

	switch (t) {
		case _INT:
			strcpy(ret, "INT");
			break;
		case _FLOAT:
			strcpy(ret, "FLOAT");
			break;
		case _CHAR:
			strcpy(ret, "CHAR");
			break;
		case _CLASS:
			strcpy(ret, "CLASS");
			break;
		case _VOID:
			strcpy(ret, "VOID");
			break;
		case _STRING:
			strcpy(ret, "STRING");
			break;
		case _ARRAY:
			strcpy(ret, "ARRAY");			
			break;
		case _UNKNOWN:
			strcpy(ret, "UNKNOWN");			
			break;
	}

	return ret;
}

void printIfStm(int id, struct IfStm * if_stm) {
	// condiditon
	printEdge(id, ++cur_id);
	printNode(cur_id, "CONDITION");
	int optid = cur_id;
	printEdge(optid, ++cur_id);
	printExpr(cur_id, if_stm->condition);

	// then statement
	printEdge(id, ++cur_id);
	printNode(cur_id, "THEN STM");
	if (if_stm->thenclause != NULL) printStmList(cur_id, if_stm->thenclause);

	// else statement
	printEdge(id, ++cur_id);
	printNode(cur_id, "ELSE STM");
	if (if_stm->elseclause != NULL) printStmList(cur_id, if_stm->elseclause);
}

void printDoStm(int id, struct DoStm * do_stm) {
	// condiditon
	printEdge(id, ++cur_id);
	printNode(cur_id, "CONDITION");
	int optid = cur_id;
	printEdge(optid, ++cur_id);
	printExpr(cur_id, do_stm->condition);		
	// body
	printEdge(id, ++cur_id);
	printNode(cur_id, "BODY");
	if (do_stm->body != NULL) printStmList(cur_id, do_stm->body);
}

void printWhileStm(int id, struct WhileStm * while_stm) {
	// condiditon
	printEdge(id, ++cur_id);
	printNode(cur_id, "CONDITION");
	int optid = cur_id;
	printEdge(optid, ++cur_id);
	printExpr(cur_id, while_stm->condition);
	// body
	printEdge(id, ++cur_id);
	printNode(cur_id, "BODY");
	if (while_stm->body != NULL) printStmList(cur_id, while_stm->body);
}

void printSwitchStm(int id, struct SwitchStm *switch_stm) {
	// condiditon
	printEdge(id, ++cur_id);
	printNode(cur_id, "CONDITION");
	int optid = cur_id;
	printEdge(optid, ++cur_id);
	printExpr(cur_id, switch_stm->condition);
	// body
	printEdge(id, ++cur_id);
	printNode(cur_id, "BODY");
	if (switch_stm->body != NULL) printStmList(cur_id, switch_stm->body);
}

void printCaseStm(int id, struct CaseStm *case_stm) {
	// condiditon
	printEdge(id, ++cur_id);
	printNode(cur_id, "OPTION");
	int optid = cur_id;
	printEdge(optid, ++cur_id);
	printExpr(cur_id, case_stm->option);		
	// body
	printEdge(id, ++cur_id);
	printNode(cur_id, "BODY");
	if (case_stm->body != NULL) printStmList(cur_id, case_stm->body);	
}

void printForStm(int id, struct ForStm *for_stm) {
	// print variable
	printEdge(id, ++cur_id);
	printVar(cur_id, for_stm->variable);
	// print expr 1
	if (for_stm->expr1 != NULL) {
		printEdge(id, ++cur_id);		
		printExpr(cur_id, for_stm->expr1);		
	}	
	// print expr 2
	if (for_stm->expr2 != NULL) {
		printEdge(id, ++cur_id);		
		printExpr(cur_id, for_stm->expr2);		
	}		
	// body
	printEdge(id, ++cur_id);
	printNode(cur_id, "BODY");
	if (for_stm->body != NULL) printStmList(cur_id, for_stm->body);	
}
#endif