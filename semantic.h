#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "semantic_struct.h"

bool doSemantic(struct Program * p) {
	bool ret_stm;
	
	semantic = initSemantic();
	
	if (checkSemanticStmList(p->stm_list, semantic->field_table)) {		
		// create const table
		createConstTable(p);		
		addConstFunction();

		file = fopen("semantic.txt", "w");
		printConstTable(file);
		printMethodTable(file);
		printVarTable(file, semantic->field_table);

		fclose(file);

		return true;
	}
	else {	
		return false;
	}
}

void printMethodTable(FILE *file) {
	fprintf(file, "METHOD TABLE\n");
	fprintf(file, "No.\tName\t\tName_Id\tDesc_Id\n");

	int i = 1;
	for (struct MethodTableConstant *iter = semantic->method_table->first; iter != NULL; iter = iter->next, ++i) {
		fprintf(file, "%d\t%s\t%d\t%d\n",i, iter->name, iter->name_index, iter->desc_index);
	}
}

void printVarTable(FILE *file, struct VarTable *var_table) {	
	fprintf(file, "Field Table\n");
	fprintf(file, "No.\tName\t\tName_Id\tDesc_Id\n");
	int i = 0;
	for (struct VarTableConst *iter = var_table->first; iter != NULL; iter = iter->next, ++i)
		fprintf(file, "%d\t%s\t\t%d\t%d\n", i, iter->name, iter->name_index, iter->desc_index);
}

void printConstTable(FILE *file) {
	fprintf(file, "CONSTANT TABLE\n");	
	struct SemanticConstant * iter = semantic->const_table->first;	
	fprintf(file, "No.\tType\t\tValue\n");
	int row = 0;
	while (iter != NULL) {
		row++;
		fprintf(file, "%d\t", row);
		printConst(file, iter);
		iter = iter->next;
		fprintf(file, "\n");
	}
}

void printConst(FILE * file, struct SemanticConstant * c) {	
	switch (c->type) {
		case CONSTANT_Utf8:
			fprintf(file, "utf8\t\t%s", c->utf8_val);
			break;
		case CONSTANT_Integer:
			fprintf(file, "int\t\t%d", c->int_val);			
			break;
		case CONSTANT_Float:
			fprintf(file, "float\t\t%.2f", c->float_val);
			break;
		case CONSTANT_Class:
			fprintf(file, "class\t\t%d", c->link1);
			break;			
		case CONSTANT_String:
			fprintf(file, "string\t\t%d", c->link1);
			break;
		case CONSTANT_Fieldref:
			fprintf(file, "fieldref\t%d,%d", c->link1, c->link2);
			break;
		case CONSTANT_Methodref:
			fprintf(file, "methodref\t%d,%d", c->link1, c->link2);
			break;
		case CONSTANT_NameAndType:
			fprintf(file, "name&type\t%d,%d", c->link1, c->link2);
			break;
	}
}

void addStmListToConstTable(struct StatementList * stmlist) {	
	struct Statement *iter = stmlist->first;
	while (iter != NULL) {
		switch (iter->type) {
			case _VAR_DECL_STM:						
				addVarsToConstTable(iter->var_decl->var_list, _LOCAL);
				break;
			case _EXPR_STM:
				addExprToConstTable(iter->expr_stm);
				break;	
			case _IF_STM:
				addExprToConstTable(iter->if_stm->condition);
				if (iter->if_stm->thenclause != NULL) addStmListToConstTable(iter->if_stm->thenclause);
				if (iter->if_stm->elseclause != NULL) addStmListToConstTable(iter->if_stm->elseclause);
				break;
			case _DO_STM:
				addExprToConstTable(iter->do_stm->condition);
				if (iter->do_stm->body != NULL)	addStmListToConstTable(iter->do_stm->body);
				break;
			case _WHILE_STM:
				addExprToConstTable(iter->while_stm->condition);
				if (iter->while_stm->body != NULL) addStmListToConstTable(iter->while_stm->body);
				break;				
			case _FOR_STM:
				if (iter->for_stm->variable != NULL) addVarsToConstTable(iter->for_stm->variable, _LOCAL);
				if (iter->for_stm->expr1 != NULL) addExprToConstTable(iter->for_stm->expr1);
				if (iter->for_stm->expr2 != NULL) addExprToConstTable(iter->for_stm->expr2);
				if (iter->for_stm->body != NULL) addStmListToConstTable(iter->for_stm->body);
				break;
			case _SWITCH_STM:
				if (iter->switch_stm->body != NULL) addStmListToConstTable(iter->switch_stm->body);
				break;
			case _CASE_STM:
				addExprToConstTable(iter->case_stm->option);		
				if (iter->case_stm->body != NULL) addStmListToConstTable(iter->case_stm->body);
				break;
			case _RETURN_STM:
				addExprToConstTable(iter->expr_stm);
				break;	
			default:
				// do nothing
				break;
		}

		iter = iter->next;
	}
}

void addExprToConstTable(struct Expression *expr) {	
	int str_no;
	struct Expression *iter;
	switch (expr->type) {
		case _CHVAL:
			addToConstTable(CONSTANT_Integer, (void *)&expr->cval, NULL);
			break;
		case _IVAL:
			addToConstTable(CONSTANT_Integer, (void *)&expr->ival, NULL);
			break;
		case _FVAL:
			addToConstTable(CONSTANT_Float, (void *)&expr->fval, NULL);
			break;
		case _SVAL:
			str_no = addToConstTable(CONSTANT_Utf8, (void *)expr->sval, NULL);
			addToConstTable(CONSTANT_String, (void *)&str_no, NULL);
			break;			
		case _IDVAL:
		case _READLN:		
			// do nothing
			break;		
		case _FUNCALL:
			iter = expr->left;
			while (iter != NULL) {
				addExprToConstTable(iter);
				iter = iter->next;
			}
		case _POSTFIX_ASS:
			addExprToConstTable(expr->left);
			addExprToConstTable(expr->mid);
			addExprToConstTable(expr->right);
			break;
		case _CASTING_TYPE:
		case _ADD_ADD_B:
		case _SUB_SUB_B:
		case _NOT:
		case _U_MINUS:
		case _WRITELN:
			addExprToConstTable(expr->left);
			break;
		default:
			addExprToConstTable(expr->left);
			addExprToConstTable(expr->right);
			break;	
	}
}

void addFuncDeclToConstTable(struct FunctionDecl *func_decl) {
	int name_no = addToConstTable(CONSTANT_Utf8, (void *)func_decl->name, NULL);	
	char *desc = getFuncDescriptor(func_decl->ret_type, func_decl->ret_dim, func_decl->arg_list);
	int desc_no = addToConstTable(CONSTANT_Utf8, (void*)desc, NULL);
	int nat_no = addToConstTable(CONSTANT_NameAndType, (void *)&name_no, (void *)&desc_no);		
	int class_no = 2;

	int method_no = addToConstTable(CONSTANT_Methodref, (void *)&class_no, (void *)&nat_no);

	if (strcmp(func_decl->name, "dlangmain") == 0) {
		const_id.dlang_main = method_no;
	}

	struct MethodTableConstant *method = findMethod(func_decl->name);
	method->name_index = name_no;
	method->desc_index = desc_no;
}

void addVarsToConstTable(struct Variable *vars, enum Flag f) {
	if (vars->name == NULL) return ;

	int name_no, desc_no, class_no = 2, nametype_no;
	struct Variable *iter = vars;
	struct VarTableConst *row;
	while (iter != NULL) {		
		if (f == _GLOBAL) {
			// add variable name to table (utf 8)		
			name_no = addToConstTable(CONSTANT_Utf8, (void *)iter->name, NULL);		
			// create descriptor and add it to table (utf8)
			char *desc = getVarDescriptor(iter->type, iter->dim);
			desc_no = addToConstTable(CONSTANT_Utf8, (void*)desc, NULL);
			// create name and type and add it to table
			nametype_no = addToConstTable(CONSTANT_NameAndType, (void *)&name_no, (void *)&desc_no);
			// add var to table		
			addToConstTable(CONSTANT_Fieldref, (void *)&class_no, (void *)&nametype_no);

			row = findVar(iter->name, semantic->field_table);
			row->name_index = name_no;
			row->desc_index = desc_no;
		}

		if (iter->init_expr != NULL) {
			addExprToConstTable(iter->init_expr);		
		}
		iter = iter->next; // go to next variable
	}
}

char *getStrFromType(enum BasicType t) {
	char *ret = (char *)malloc(50 * sizeof(char));
	switch (t) {
		case _CHAR:
		case _INT:
			strcpy(ret, "I");
			break;
		case _FLOAT:
			strcpy(ret, "F");
			break;					
		case _VOID:
			strcpy(ret, "V");
			break;
		case _STRING:
			strcpy(ret, "Ljava/lang/String;");
			break;
		case _UNKNOWN:
		default:
			ret = NULL;
			break;
	}
	return ret;
}

char *getVarDescriptor(enum BasicType type, struct Dimention * dim) {
	char *ret = (char *)malloc(40 * sizeof(char));
	strcpy(ret, "");
	// work with dimention first
	struct Dimention *iter = dim;
	while (iter != NULL) {
		strcat(ret, "[");
		iter = iter->next;
	}
	// work with type
	strcat(ret, getStrFromType(type));

	return ret;
}

char *getFuncDescriptor(enum BasicType type, struct Dimention * dim, struct Variable * vars) {
	char *ret = (char *)malloc(40 * sizeof(char));
	strcpy(ret, "(");
	// analyse parameters
	struct Variable *iter = vars;
	while (iter != NULL) {
		char *var_des = getVarDescriptor(iter->type, iter->dim);
		strcat(ret, var_des);
		iter = iter->next;
	}
	strcat(ret, ")");
	strcat(ret, getStrFromType(type));	
	return ret;
}

void createConstTable(struct Program *prog) {		
	int n = addToConstTable(CONSTANT_Utf8, (void *)semantic->name, NULL);
	const_id.this_class = addToConstTable(CONSTANT_Class, (void *)&n, NULL);

	struct Statement *iter = prog->stm_list->first;
	while (iter != NULL) {
		switch (iter->type) {
			case _VAR_DECL_STM:						
				addVarsToConstTable(iter->var_decl->var_list, _GLOBAL);
				break;
			case _FUNC_DECL_STM:
				addFuncDeclToConstTable(iter->fun_decl);
				break;
			case _FUNC_DEF_STM:
				addFuncDeclToConstTable(iter->fun_def->header);
				addStmListToConstTable(iter->fun_def->body);
				break;
			default:
				break;
		}

		iter = iter->next;
	}
}

int findConstant(enum SemanticConstantType type, void *ptr1, void *ptr2) {			
	int pos = 0;
	struct SemanticConstant *row = semantic->const_table->first;
	while (row != NULL) {
		pos++;
		if (row->type == type) {
			switch (row->type) {		
				case CONSTANT_Utf8:				
					if (strcmp(row->utf8_val, (char *)ptr1) == 0) return pos;
					break;
				case CONSTANT_Integer:
					if (row->int_val == *((int*)ptr1)) return pos;
					break;
				case CONSTANT_Float:
					if (row->float_val == *((float*)ptr1)) return pos;
					break;
				case CONSTANT_Class:
					if (row->link1 == *((int*)ptr1)) return pos;
					break;			
				case CONSTANT_String:
					if (row->link1 == *((int*)ptr1)) return pos;
					break;
				case CONSTANT_Fieldref:
				case CONSTANT_Methodref:
				case CONSTANT_NameAndType:		
					if (row->link1 == *((int*)ptr1) && row->link2 == *((int*)ptr2)) return pos;					
					break;
			}	
		}
		// jump to next row
		row = row->next;		
	}
	return -1;
}

int addToConstTable(enum SemanticConstantType type, void *ptr1, void *ptr2) {		
	struct SemanticConstant *row = (struct SemanticConstant *)malloc(sizeof(struct SemanticConstant));
	row->next = NULL;
	row->type = type;		
	int pos = findConstant(type, ptr1, ptr2);
	if (pos >= 0) return pos;

	switch (type) {
		case CONSTANT_Utf8:
			row->utf8_val = (char *)ptr1;			
			break;
		case CONSTANT_Integer:
			row->int_val = *((int*)ptr1);
			break;
		case CONSTANT_Float:
			row->float_val = *((float*)ptr1);
			break;
		case CONSTANT_Class:
			row->link1 = *((int*)ptr1);
			break;			
		case CONSTANT_String:
			row->link1 = *((int*)ptr1);
			break;
		case CONSTANT_Fieldref:
		case CONSTANT_Methodref:
		case CONSTANT_NameAndType:
			row->link1 = *((int*)ptr1);
			row->link2 = *((int*)ptr2);
			break;
	}
	// add row to table
	if (semantic->const_table->first == NULL) {		
		semantic->const_table->first = row;
		semantic->const_table->last = row;
	}
	else {
		semantic->const_table->last->next = row;
		semantic->const_table->last = row;
	}

	return ++(semantic->const_table->size);
}

struct MethodTableConstant *checkSemanticFundef(struct FunctionDef *fun_def) {	
	struct MethodTableConstant *method = findMethod(fun_def->header->name);
	if (method == NULL) {		
		method = addToMethodTable(fun_def->header);
		method->body = fun_def->body;
	}

	if (!checkSemanticStmList(fun_def->body, method->local_table)) {
		return NULL;
	}
	else {		
		return method;		
	}
}

bool checkSemanticStmList(struct StatementList *stmlist, struct VarTable *var_table) {
	if (stmlist == NULL) return true;
	// init new table
	bool ret_stm;
	struct SemanticType ret_expr;

	for (struct Statement *stm = stmlist->first; stm != NULL; stm = stm->next) {
		switch (stm->type) {
			case _VAR_DECL_STM:				
				if (!checkSemanticVarList(stm->var_decl->var_list, var_table)) return false;		
				break;
			case _FUNC_DECL_STM:
				addToMethodTable(stm->fun_decl);									
			 	break;				
			case _FUNC_DEF_STM:
				if (checkSemanticFundef(stm->fun_def) == NULL)
					return false;				
				break;
			case _EXPR_STM:				
				ret_expr = checkSemanticExpr(stm->expr_stm, var_table);												
				if (ret_expr.type == _UNKNOWN) return false;
				break;
			case _IF_STM:
				ret_expr = checkSemanticExpr(stm->if_stm->condition, var_table);			
				if (ret_expr.type == _UNKNOWN) return false;
				ret_stm = checkSemanticStmList(stm->if_stm->thenclause, var_table);
				if (ret_stm == false) return false;
				ret_stm = checkSemanticStmList(stm->if_stm->elseclause, var_table);
				if (ret_stm == false) return false;
				break;
			case _DO_STM:
				ret_expr = checkSemanticExpr(stm->do_stm->condition, var_table);
				if (ret_expr.type == _UNKNOWN) return false;
				ret_stm = checkSemanticStmList(stm->do_stm->body, var_table);			
				if (ret_stm == false) return false;
				break;
			case _WHILE_STM:
				ret_expr = checkSemanticExpr(stm->while_stm->condition, var_table);
				if (ret_expr.type == _UNKNOWN) return false;
				ret_stm = checkSemanticStmList(stm->while_stm->body, var_table);			
				if (ret_stm == false) return false;
				break;
			case _FOR_STM:
				if (stm->for_stm->variable->type != _UNKNOWN){
					ret_stm = checkSemanticVarList(stm->for_stm->variable, var_table);
					if (ret_stm == false) return false;
				}
				else {					
					ret_expr = checkSemanticExpr(stm->for_stm->variable->init_expr, var_table);
					if (ret_expr.type == _UNKNOWN) return false;
				}

				ret_expr = checkSemanticExpr(stm->for_stm->expr1, var_table);
				if (ret_expr.type == _UNKNOWN) return false;
				ret_expr = checkSemanticExpr(stm->for_stm->expr2, var_table);
				if (ret_expr.type == _UNKNOWN) return false;
				ret_stm = checkSemanticStmList(stm->for_stm->body, var_table);	
				if (ret_stm == false) return false;
				break;
			case _SWITCH_STM:
				// ret_expr = checkSemanticExpr(stm->switch_stm->condition, var_table);
				// if (ret_expr.type == _UNKNOWN) return false;
				// ret_stm = checkSemanticStmList(stm->switch_stm->body, var_table);	
				// if (ret_stm == false) return false;
				// break;
			case _CASE_STM:
				// ret_expr = checkSemanticExpr(stm->case_stm->option, var_table);
				// if (ret_expr.type == _UNKNOWN) return false;
				// ret_stm = checkSemanticStmList(stm->case_stm->body, var_table);			
				// if (ret_stm == false) return false;
				// break;
			case _RETURN_STM:
				ret_expr = checkSemanticExpr(stm->expr_stm, var_table);
				if (ret_expr.type == _UNKNOWN) return false;
				break;
			default:
				break;
		}
	}
	return true;
}

bool isEqualSemantic(struct SemanticType st1, struct SemanticType st2) {
	return (st1.dim_count == st2.dim_count) && (st1.type == st2.type);
}

bool checkSemanticVarList(struct Variable *vars, struct VarTable *var_table) {	
	struct SemanticType expr_se_type;
	int ndim;
	for (struct Variable *iter = vars; iter != NULL; iter = iter->next) {
		iter->semantic_type = getSemanticType(iter->type, iter->dim);		

		// find variable in table and add it to table
		if (findVar(iter->name, var_table) == NULL) {
			addToVarTable(iter, var_table);
		}
		else {			
			printf("Line %d: variable %s is already declared\n", iter->line, iter->name);
			return false;
		}			
		
		// define semantic type of init expression
		if (iter->init_expr != NULL) {
			expr_se_type = checkSemanticExpr(iter->init_expr, var_table);	
			// check type missmatch
			if (!isEqualSemantic(expr_se_type, iter->semantic_type)) {
				if ((iter->semantic_type.type == _INT && expr_se_type.type == _FLOAT) || (iter->semantic_type.type == _FLOAT && expr_se_type.type == _INT)) 
					printf("Line %d: warning: auto cast between int and float\n", iter->line);
				else {
					printf("Line %d: type missmatch in variable %s declaration\n", iter->line, iter->name);	
					return false;
				}
			} 
			// prevent declare array with any kind of initinaze, except string
			if (iter->semantic_type.type == _ARRAY) {
				printf("Line %d: init is prevented in variable %s\n", iter->line, iter->name);
				return false;
			}
		}
	}	

	return true;
}

struct SemanticType checkSemanticExpr(struct Expression * expr, struct VarTable *var_table) {
	struct SemanticType left, right, mid;

	struct SemanticType st;
	st.type = _UNKNOWN;
	st.dim_count = 0;	
	st.basic_type = _UNKNOWN;	

	struct MethodTableConstant *method;
	struct VarTableConst *field;
	struct Expression *expr_iter;
	int i;

	switch (expr->type) {
		case _CHVAL:
			st.type = _CHAR;
			st.dim_count = 0;
			st.basic_type = _UNKNOWN;
			expr->semantic_type = st;
			return st;
		case _IVAL:
			st.type = _INT;
			st.dim_count = 0;
			st.basic_type = _UNKNOWN;
			expr->semantic_type = st;
			return st;
		case _FVAL:
			st.type = _FLOAT;
			st.dim_count = 0;
			st.basic_type = _UNKNOWN;
			expr->semantic_type = st;
			return st;
		case _SVAL:
			st.type = _STRING;
			st.dim_count = 1;
			st.basic_type = _CHAR;
			expr->semantic_type = st;
			return st;
		case _IDVAL:
			field = findVar(expr->sval, var_table);

			if (field == NULL) {				

				field = findVar (expr->sval, semantic->field_table);

				if (field == NULL) {
					printf("Line %d: name %s is not defined\n", expr->line, expr->sval);				
					return st;									
				}
			} 

			expr->semantic_type = field->semantic_type;
			return expr->semantic_type;
		case _SUB:	
		case _ADD:
		case _MUL:
		case _DIV:
		case _SUB_ASS:
		case _ADD_ASS:
		case _MUL_ASS:
		case _DIV_ASS:			
			left = checkSemanticExpr(expr->left, var_table);
			right = checkSemanticExpr(expr->right, var_table);

			if (left.type == _UNKNOWN || right.type == _UNKNOWN) return st;

			// doesn't work with ARRAY
			if (left.type == _ARRAY || right.type == _ARRAY) {
				printf("Line %d: operand %s can't work with array\n", expr->line, getSymbol(expr->type));
				return st;
			}
			// + - * / : cast to FLOAT if one is FLOAT
			if (left.type == _FLOAT && right.type == _INT) {				
				printf("before %d %d\n", expr->left->semantic_type.type, expr->right->semantic_type.type);
				addCastExpr(expr, left, _RIGHT);
				printf("after %d %d\n", expr->left->semantic_type.type, expr->right->semantic_type.type);
				expr->semantic_type = left;
				return left;
			}

			if (left.type == _INT && right.type == _FLOAT) {
				addCastExpr(expr, right, _LEFT);
				expr->semantic_type = right;
				return right;
			}

			if (left.type == _INT && right.type == _CHAR) {
				addCastExpr(expr, left, _RIGHT);
				expr->semantic_type = left;
				return left;
			}	

			if (isEqualSemantic(left, right)) {
				expr->semantic_type = left;
				return left;
			}

			printf("Line %d: operand %s require same operators type\n", expr->line, getSymbol(expr->type));
			return st;
		case _ASS:			
			left = checkSemanticExpr(expr->left, var_table);
			right = checkSemanticExpr(expr->right, var_table);

			if (left.type == _UNKNOWN || right.type == _UNKNOWN) return st;

			// if (expr->left->type != _IDVAL) {
			// 	printf("Line: %d operand %s require left operator is variable\n", expr->line, getSymbol(expr->type));
			// 	return st;
			// }

			// casting 
			if (left.type == _FLOAT && right.type == _INT) {							
				addCastExpr(expr, left, _RIGHT);
				expr->semantic_type = left;
				return left;
			}

			if (left.type == _INT && right.type == _FLOAT) {
				addCastExpr(expr, right, _LEFT);
				expr->semantic_type = right;
				return right;
			}

			// transform with []= 
			if (expr->left->type == _POSTFIX) addPosfixAssExpr(expr);

			if (left.type == _INT && right.type == _CHAR) {
				addCastExpr(expr, left, _RIGHT);
				expr->semantic_type = right;
				return right;
			}	

			// only work if right and left have same type
			if (isEqualSemantic(left, right)) {
				expr->semantic_type = left;	
				return left;
			}

			printf("Line: %d operand %s require same operators type\n", expr->line, getSymbol(expr->type));
			return st;
		case _CAT:
		case _CAT_ASS:
			left = checkSemanticExpr(expr->left, var_table);
			right = checkSemanticExpr(expr->right, var_table);
			if (left.type == _UNKNOWN || right.type == _UNKNOWN) return st;

			// ~ only work with two string (char[])
			if (left.type != _STRING || right.type != _STRING) {
				printf("Line %d: operand %s require two strings.\n", expr->line, getSymbol(expr->type));
				return st;
			} 

			expr->semantic_type = left;
			return left;
		case _OR_OR:
		case _AND_AND:
			left = checkSemanticExpr(expr->left, var_table);
			right = checkSemanticExpr(expr->right, var_table);			
			if (left.type == _UNKNOWN || right.type == _UNKNOWN) return st;

			// && and || only work with int and char
			if ((left.type == _INT || left.type == _CHAR) && (right.type == _INT || right.type == _CHAR)) {
				st.type = _INT;
				st.dim_count = 0;
				st.basic_type = _UNKNOWN;			
				expr->semantic_type = st;
				return st;
			}

			printf("Line %d: operand %s require two integers.\n", expr->line, getSymbol(expr->type));
			return st;
		case _NOT:
		case _ADD_ADD_B:
		case _SUB_SUB_B:
			// ! ++ and -- only work with int
			left = checkSemanticExpr(expr->left, var_table);
			if (left.type == _UNKNOWN) return st;
			
			if (left.type != _INT) {
				printf("Line %d: operand %s require integer.\n", expr->type, getSymbol(expr->type));
				return st;
			} 

			expr->semantic_type = left;	
			return left;
		case _EQ:
		case _GT:
		case _LT:
		case _GE:
		case _LE:
		case _NEQ:
			left = checkSemanticExpr(expr->left, var_table);
			right = checkSemanticExpr(expr->right, var_table);
			if (left.type == _UNKNOWN || right.type == _UNKNOWN) return st;

			// doesn't work with ARRAY
			if (left.type == _ARRAY || right.type == _ARRAY) {
				printf("Line %d: operand %s can not compare two arrays\n", expr->line, getSymbol(expr->type));
				return st;
			}

			// compare to FLOAT if one is float
			if (left.type == _FLOAT && right.type == _INT) {
				addCastExpr(expr, left, _RIGHT);
				expr->semantic_type = left;
				return left;
			}

			if (left.type == _INT && right.type == _FLOAT) {
				addCastExpr(expr, right, _LEFT);
				expr->semantic_type = right;
				return right;
			}

			if (left.type == _INT && right.type == _CHAR) {
				addCastExpr(expr, left, _RIGHT);
				expr->semantic_type = left;
				return left;
			}			

			if (left.type == _CHAR && right.type == _FLOAT) {
				addCastExpr(expr, right, _LEFT);
				return right;
			}

			// any way, compare require 2 same type
			if (isEqualSemantic(left, right)) {
				expr->semantic_type = left;
				return left;
			}

			printf("Line %d: operand %s require same operators type\n",expr->line, getSymbol(expr->type));
			return st;
		case _U_MINUS:
			left = checkSemanticExpr(expr->left, var_table);
			if (left.type == _UNKNOWN) return st;

			// doesn't work with STRING and ARRAY
			if (left.type == _STRING || left.type == _ARRAY) {
				printf("Line %d: operand %s can't work with array or string\n", expr->line, getSymbol(expr->type));
				return st;	
			} 

			expr->semantic_type = left;	
			return left;	
		case _POSTFIX:
			left = checkSemanticExpr(expr->left, var_table);
			right = checkSemanticExpr(expr->right, var_table);
			if (left.type == _UNKNOWN || right.type == _UNKNOWN) return st;

			// only work with left is array and right is int
			if ((left.type == _ARRAY || left.type == _STRING) && right.type == _INT) {
				st.dim_count = left.dim_count - 1;

				if (st.dim_count == 0) {
					st.type = left.basic_type;
					st.basic_type = _UNKNOWN;
				}
				else 
					st.type = _ARRAY;

				expr->semantic_type = st;
				return st;
			}

			if (right.type != _INT) {
				printf("Line %d: array id must be int\n", expr->line);
			}

			if (left.type != _ARRAY && left.type != _STRING) {
				printf("Line %d: left expression must be array\n", expr->line);
			}

			return st;
		case _POSTFIX_ASS:
			left = checkSemanticExpr(expr->left, var_table);
			right = checkSemanticExpr(expr->right, var_table);
			mid = checkSemanticExpr(expr->mid, var_table);

			if (left.type == _UNKNOWN || right.type == _UNKNOWN || mid.type == _UNKNOWN) return st;
			
			// only work with left is array and right is int
			if ((left.type == _ARRAY || left.type == _STRING) && mid.type == _INT) {
				st.dim_count = left.dim_count - 1;
				st.basic_type = left.basic_type;

				if (st.dim_count == 0) {
					st.type = left.basic_type;
					st.basic_type = _UNKNOWN;
				} else
					st.type = _ARRAY;

				if (!isEqualSemantic(st, right)) {
					printf("Line %d: operand = require same type left and right\n", expr->line);
					st.type = _UNKNOWN;
					st.basic_type = _UNKNOWN;
					st.dim_count = 0;
					return  st;
				}

				return st;
			}

			if (mid.type != _INT) {
				printf("Line %d: array id must be int\n", expr->line);
			}

			if (left.type != _ARRAY && left.type != _STRING) {
				printf("Line %d: left expression must be array\n", expr->line);
			}

			return st;
		case _FUNCALL:			
			method = findMethod(expr->sval);
			if (method == NULL) {
				printf("Line %d: function %s is not declared\n",expr->line, expr->sval);
				return st;
			}			

			expr_iter = expr->left;
			expr->next_expr_count = 0;
			
			while (expr_iter != NULL) {

				left = checkSemanticExpr(expr_iter, var_table);

				if (left.type == _UNKNOWN) return st;

				printf("found: %d\n", left.type);
				printf("require: %d\n", method->param_type[expr->next_expr_count].type);

				if (left.type == _INT && method->param_type[expr->next_expr_count].type == _FLOAT) {
					addCastExpr(expr, right, _LEFT);					
				} else {
					if (!isEqualSemantic(left, method->param_type[expr->next_expr_count])) {
						printf("Line %d: type is not match at arguments %d of function %s\n",expr->line, expr->next_expr_count + 1, expr->sval);	
						return st;
					}					
				}

				expr->next_expr_count++;
				if (expr->next_expr_count + 1 > method->param_count) {
					printf("Line %d, too much arguments in function call %s\n", expr->line, expr->sval);
					return st;	
				}
				expr_iter = expr_iter->next;
			}	


			if (expr->next_expr_count < method->param_count) {
				printf("Line %d, too few arguments in function call %s\n", expr->line, expr->sval);
				return st;	
			}

			st.type = method->return_type.type;
			st.dim_count = method->return_type.dim_count;
			st.basic_type = method->return_type.basic_type;
			expr->semantic_type = st;
			return st;

		case _READLN:
		case _WRITELN:			
			left = checkSemanticExpr(expr->left, var_table);
			if (left.type == _ARRAY) {
				printf("Line %d: function writeln can't work with array\n", expr->line);
				return st;		
			}

			st.type = _VOID;
			st.dim_count = 0;
			st.basic_type = _UNKNOWN;
			expr->semantic_type = st;
			return st;		
		default:
			return st;
	}
}	

struct VarTableConst *findVar(char *id, struct VarTable *var_table) {	
	for (struct VarTableConst *iter = var_table->first; iter != NULL; iter = iter->next) {		
		if (strcmp(iter->name, id) == 0) 
			return iter;
	}
	return NULL;
}

void addToVarTable(struct Variable * var, struct VarTable *var_table) {
	struct VarTableConst *node = (struct VarTableConst*)malloc(sizeof(struct VarTableConst));
	node->semantic_type = var->semantic_type;	
	node->name = var->name;
	node->next = NULL;
	node->index = var_table->size;

	if (var_table == semantic->field_table)
		node->index += 1;		
	
	if (var_table->first == NULL) {
		var_table->first = node;
		var_table->last = node;
	} else {
		var_table->last->next = node;
		var_table->last = node;	
	}
	var_table->size++;	
}

void addCastExpr(struct Expression *e, struct SemanticType t, enum Direction d) {
	struct Expression *newNode = (struct Expression *) malloc(sizeof(struct Expression));
	newNode->next = NULL;
	newNode->semantic_type = t;
	newNode->type = _CASTING_TYPE;
	newNode->left = (d == _RIGHT) ? e->right : e->left;
	if (d == _RIGHT)
		e->right = newNode;
	else
		e->left = newNode;
}

void addPosfixAssExpr(struct Expression *e) {
	struct Expression *left = e->left->left;
	struct Expression *right = e->left->right;

	e->type = _POSTFIX_ASS;
	e->mid = right;
	e->left = left;
}

struct MethodTableConstant *addToMethodTable(struct FunctionDecl *func_decl) {		
	struct MethodTableConstant *method = (struct MethodTableConstant*)malloc(sizeof(struct MethodTableConstant));
	method->next = NULL;
	method->name = func_decl->name;
	method->body = NULL;
	method->local_table = (struct VarTable *)malloc(sizeof(struct VarTable));
	method->local_table->first = NULL;
	method->local_table->last = NULL;
	method->local_table->size = 0;

	method->return_type = getSemanticType(func_decl->ret_type, func_decl->ret_dim);

	// rename main function
	if (strcmp(func_decl->name, "main") == 0) {
		strcpy(func_decl->name, "dlangmain");
	}

	// count parameters
	method->param_count = 0;
	struct Variable *iter = func_decl->arg_list;
	while (iter != NULL) {
		method->param_count++;
		iter = iter->next;
	}
	method->param_type = (struct SemanticType *)malloc(method->param_count * sizeof(struct SemanticType));

	// analyse parameters
	int i;
	for (iter = func_decl->arg_list; iter != NULL; iter = iter->next, i++) {
		method->param_type[i] = getSemanticType(iter->type, iter->dim);
	}

	// add to method table
	if (semantic->method_table->first == NULL) {
		semantic->method_table->first = method;
		semantic->method_table->last = method;
	} else {
		semantic->method_table->last->next = method;
		semantic->method_table->last = method;	
	}
	semantic->method_table->size++;	

	return method;
}

struct MethodTableConstant *findMethod(char * id) {
	struct MethodTableConstant *iter = semantic->method_table->first;
	while (iter != NULL) {
		if (strcmp(iter->name, id) == 0) return iter;
		iter = iter->next;
	}
	return NULL;
}

struct SemanticType getSemanticType(enum BasicType type, struct Dimention *dim) {
	struct SemanticType st;	
	st.type = type;
	st.basic_type = type;
	st.dim_count = 0;
	// ret type
	if (dim != NULL) {
		for (struct Dimention *iter = dim; iter != NULL; iter = iter->next) {
			st.dim_count++;	
		}

		if (type == _CHAR && dim->next == NULL)
			st.type = _STRING;
		else
			st.type = _ARRAY;
	} 
	return st;
}

struct SemanticClass *initSemantic() {
	struct SemanticClass *ret = (struct SemanticClass *)malloc(sizeof(struct SemanticClass));
	ret->name = (char *)malloc(strlen("DLang") * sizeof(char));
	strcpy(ret->name, "DLang");

	// init constant table
	ret->const_table = (struct ConstantTable *)malloc(sizeof(struct ConstantTable));
	ret->const_table->first = ret->const_table->last = NULL;
	ret->const_table->size = 0;	

	// init method table
	ret->method_table = (struct MethodTable *)malloc(sizeof(struct MethodTable));
	ret->method_table->first = ret->method_table->last = NULL;
	ret->method_table->size = 0;	

	// init field table
	ret->field_table = (struct VarTable *)malloc(sizeof(struct VarTable));
	ret->field_table->first = ret->field_table->last = NULL;
	ret->field_table->size = 0;	
	return ret;
}

void addConstFunction() {
	const_id.main_func_name = addToConstTable(CONSTANT_Utf8, (void *)"main", NULL);
	const_id.main_func_type = addToConstTable(CONSTANT_Utf8, (void *)"([Ljava/lang/String;)V", NULL);

	const_id.object_name = addToConstTable(CONSTANT_Utf8, (void *)"java/lang/Object", NULL);
	const_id.object_class = addToConstTable(CONSTANT_Class, &const_id.object_name, NULL);

	const_id.constructor_name = addToConstTable(CONSTANT_Utf8, (void *)"<init>", NULL);
	const_id.constructor_type = addToConstTable(CONSTANT_Utf8, (void *)"()V", NULL);
	const_id.constructor_nat = addToConstTable(CONSTANT_NameAndType, &const_id.constructor_name, &const_id.constructor_type);
	const_id.constructor_ref = addToConstTable(CONSTANT_Methodref, &const_id.object_class, &const_id.constructor_nat);

	const_id.system_name = addToConstTable(CONSTANT_Utf8, (void *)"java/lang/System", NULL);
	const_id.system_class = addToConstTable(CONSTANT_Class, &const_id.system_name, NULL);

	const_id.out_name = addToConstTable(CONSTANT_Utf8, (void *)"out", NULL);

	const_id.printstream_name = addToConstTable(CONSTANT_Utf8, (void *)"java/io/PrintStream", NULL);
	const_id.printstream_class = addToConstTable(CONSTANT_Class, &const_id.printstream_name, NULL);
	const_id.printstream_type = addToConstTable(CONSTANT_Utf8, (void *)"Ljava/io/PrintStream;", NULL);
	const_id.printstream_nat = addToConstTable(CONSTANT_NameAndType, &const_id.out_name, &const_id.printstream_type);

	const_id.print_field = addToConstTable(CONSTANT_Fieldref, &const_id.system_class, &const_id.printstream_nat);

	const_id.print_name = addToConstTable(CONSTANT_Utf8, (void *)"print", NULL);

	const_id.print_float_type = addToConstTable(CONSTANT_Utf8, (void *)"(F)V", NULL);
	const_id.print_float_nat = addToConstTable(CONSTANT_NameAndType, &const_id.print_name, &const_id.print_float_type);
	const_id.print_float_ref = addToConstTable(CONSTANT_Methodref, &const_id.printstream_class, &const_id.print_float_nat);

	const_id.print_int_type = addToConstTable(CONSTANT_Utf8, (void *)"(I)V", NULL);
	const_id.print_int_nat = addToConstTable(CONSTANT_NameAndType, &const_id.print_name, &const_id.print_int_type);
	const_id.print_int_ref = addToConstTable(CONSTANT_Methodref, &const_id.printstream_class, &const_id.print_int_nat);

	const_id.print_string_type = addToConstTable(CONSTANT_Utf8, (void *)"(Ljava/lang/String;)V", NULL);
	const_id.print_string_nat = addToConstTable(CONSTANT_NameAndType, &const_id.print_name, &const_id.print_string_type);
	const_id.print_string_ref = addToConstTable(CONSTANT_Methodref, &const_id.printstream_class, &const_id.print_string_nat);

	const_id.println_name = addToConstTable(CONSTANT_Utf8, (void *)"println", NULL);

	const_id.println_float_nat = addToConstTable(CONSTANT_NameAndType, &const_id.println_name, &const_id.print_float_type);
	const_id.println_float_ref = addToConstTable(CONSTANT_Methodref, &const_id.printstream_class, &const_id.println_float_nat);

	const_id.println_int_nat = addToConstTable(CONSTANT_NameAndType, &const_id.println_name, &const_id.print_int_type);
	const_id.println_int_ref = addToConstTable(CONSTANT_Methodref, &const_id.printstream_class, &const_id.println_int_nat);

	const_id.println_string_nat = addToConstTable(CONSTANT_NameAndType, &const_id.println_name, &const_id.print_string_type);
	const_id.println_string_ref = addToConstTable(CONSTANT_Methodref, &const_id.printstream_class, &const_id.println_string_nat);

	const_id.stringbuilder_name = addToConstTable(CONSTANT_Utf8, (void *)"java/lang/StringBuilder", NULL);
	const_id.stringbuilder_class = addToConstTable(CONSTANT_Class, &const_id.stringbuilder_name, NULL);
	const_id.stringbuilder_ref = addToConstTable(CONSTANT_Methodref, &const_id.stringbuilder_class, &const_id.constructor_nat);

	const_id.append_name = addToConstTable(CONSTANT_Utf8, (void *)"append", NULL);
	const_id.append_type = addToConstTable(CONSTANT_Utf8, (void *)"(Ljava/lang/String;)Ljava/lang/StringBuilder;", NULL);
	const_id.append_nat = addToConstTable(CONSTANT_NameAndType, &const_id.append_name, &const_id.append_type);
	const_id.append_ref = addToConstTable(CONSTANT_Methodref, &const_id.stringbuilder_class, &const_id.append_nat);

	const_id.tostring_name = addToConstTable(CONSTANT_Utf8, (void *)"toString", NULL);
	const_id.tostring_type = addToConstTable(CONSTANT_Utf8, (void *)"()Ljava/lang/String;", NULL);
	const_id.tostring_nat = addToConstTable(CONSTANT_NameAndType, &const_id.tostring_name, &const_id.tostring_type);
	const_id.tostring_ref = addToConstTable(CONSTANT_Methodref, &const_id.stringbuilder_class, &const_id.tostring_nat);

	const_id.string_name = addToConstTable(CONSTANT_Utf8, (void *)"java/lang/String", NULL);	
	const_id.string_class = addToConstTable(CONSTANT_Class, &const_id.string_name, NULL);

	const_id.length_name = addToConstTable(CONSTANT_Utf8, (void *)"length", NULL);
	const_id.length_type = addToConstTable(CONSTANT_Utf8, (void *)"()I", NULL);
	const_id.length_nat = addToConstTable(CONSTANT_NameAndType, &const_id.length_name, &const_id.length_type);
	const_id.length_ref = addToConstTable(CONSTANT_Methodref, &const_id.string_class, &const_id.length_nat);

	const_id.scanner_name = addToConstTable(CONSTANT_Utf8, (void *)"java/util/Scanner", NULL);
	const_id.scanner_class = addToConstTable(CONSTANT_Class, &const_id.scanner_name, NULL);

	const_id.in_name = addToConstTable(CONSTANT_Utf8, (void *)"in", NULL);
	const_id.in_type = addToConstTable(CONSTANT_Utf8, (void *)"Ljava/io/InputStream;", NULL);
	const_id.in_nat = addToConstTable(CONSTANT_NameAndType, &const_id.in_name, &const_id.in_type);
	const_id.in_ref = addToConstTable(CONSTANT_Fieldref, &const_id.system_class, &const_id.in_nat);

	const_id.init_inputstream_type = addToConstTable(CONSTANT_Utf8, (void *)"(Ljava/io/InputStream;)V", NULL);
	const_id.init_inputstream_nat = addToConstTable(CONSTANT_NameAndType, &const_id.constructor_name, &const_id.init_inputstream_type);
	const_id.init_inputstream_ref = addToConstTable(CONSTANT_Methodref, &const_id.scanner_class, &const_id.init_inputstream_nat);

	const_id.input_name = addToConstTable(CONSTANT_Utf8, (void *)"__input__", NULL);
	const_id.input_type = addToConstTable(CONSTANT_Utf8, (void *)"Ljava/util/Scanner;", NULL);
	const_id.input_nat = addToConstTable(CONSTANT_NameAndType, &const_id.input_name, &const_id.input_type);
	const_id.input_ref = addToConstTable(CONSTANT_Fieldref, &const_id.this_class, &const_id.input_nat);

	const_id.nextfloat_name = addToConstTable(CONSTANT_Utf8, (void *)"nextFloat", NULL);
	const_id.nextfloat_type = addToConstTable(CONSTANT_Utf8, (void *)"()F", NULL);
	const_id.nextfloat_nat = addToConstTable(CONSTANT_NameAndType, &const_id.nextfloat_name, &const_id.nextfloat_type);
	const_id.nextfloat_ref = addToConstTable(CONSTANT_Methodref, &const_id.scanner_class, &const_id.nextfloat_nat);

	const_id.nextline_name = addToConstTable(CONSTANT_Utf8, (void *)"nextLine", NULL);
	const_id.nextline_type = addToConstTable(CONSTANT_Utf8, (void *)"()Ljava/lang/String;", NULL);
	const_id.nextline_nat = addToConstTable(CONSTANT_NameAndType, &const_id.nextline_name, &const_id.nextline_type);
	const_id.nextline_ref = addToConstTable(CONSTANT_Methodref, &const_id.scanner_class, &const_id.nextline_nat);

	const_id.source_file = addToConstTable(CONSTANT_Utf8, (void *)"SourceFile", NULL);
	const_id.file = addToConstTable(CONSTANT_Utf8, (void *)"DLang.java", NULL);

	const_id.attr_code = addToConstTable(CONSTANT_Utf8, (void *)"Code", NULL);
}

#endif
