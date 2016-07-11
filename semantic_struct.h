#ifndef SEMANTIC_STRUCT_H
#define SEMANTIC_STRUCT_H

#include "tree_struct.h"
#include "tree_print.h"

// ----------------- DEFINITION STRUCTURE -----------------------

enum Flag {
	_LOCAL, _GLOBAL
};

enum SemanticConstantType {
	CONSTANT_Utf8 = 1,
	CONSTANT_Integer = 3,
	CONSTANT_Float = 4,
	CONSTANT_Class = 7,
	CONSTANT_String = 8,
	CONSTANT_Fieldref = 9,
	CONSTANT_Methodref = 10,
	CONSTANT_NameAndType = 12
};

/*Class one row in constant table*/
struct SemanticConstant {
	int id;									// no. row in table
	enum SemanticConstantType type;			// type of constant 
	int int_val;							// int val, for CONSTANT_Integer
	float float_val;						// float val, for CONSTANT_Float
	char *utf8_val;							// utf8 val
	int link1, link2;						// first and second link

	struct SemanticConstant *next;			// next row in table
};

/*Class constant table*/
struct ConstantTable {
	struct SemanticConstant *first;
	struct SemanticConstant *last;
	int size;
};

/*Class semantic*/
struct SemanticClass {
	char *name;
	struct ConstantTable *const_table;
	struct MethodTable *method_table;
	struct VarTable *field_table;
};

struct VarTableConst {
	char *name;
	struct SemanticType semantic_type;

	int name_index;
	int desc_index;
	int index;

	struct VarTableConst *next;
};

struct VarTable {
	struct VarTableConst * first;
	struct VarTableConst * last;
	int size;	
};

struct MethodTable {
	struct MethodTableConstant *first;
	struct MethodTableConstant *last;
	int size;
};

struct MethodTableConstant {
	char *name;
	struct SemanticType return_type;
	struct SemanticType *param_type;
	int param_count;

	int name_index;
	int desc_index;
	struct StatementList *body;
	struct VarTable *local_table;

	struct MethodTableConstant *next;
};

struct ConstantId {
	int this_class;
	int attr_code;
	int main_func_name;
	int main_func_type;

	int constructor_name;
	int constructor_type;
	int constructor_nat;
	int constructor_ref;

	int object_name;
	int object_class;

	int system_name;
	int system_class;

	int printstream_name;
	int printstream_type;
	int printstream_nat;
	int printstream_class;
	int printstream_ref;

	int print_field;

	int print_name;

	int print_float_type;
	int print_float_nat;
	int print_float_ref;

	int print_int_type;
	int print_int_nat;
	int print_int_ref;

	int print_string_type;
	int print_string_nat;
	int print_string_ref;

	int println_name;

	int println_float_nat;
	int println_float_ref;

	int println_int_nat;
	int println_int_ref;

	int println_string_nat;
	int println_string_ref;

	int stringbuilder_name;
	int stringbuilder_class;
	int stringbuilder_ref;

	int append_name;
	int append_type;
	int append_nat;
	int append_ref;

	int tostring_name;
	int tostring_type;
	int tostring_nat;
	int tostring_ref;

	int string_name;
	int string_class;

	int length_name;
	int length_type;
	int length_nat;
	int length_ref;

	int scanner_name;
	int scanner_class;

	int in_name;
	int in_type;
	int in_nat;
	int in_ref;

	int input_name;
	int input_type;
	int input_nat;
	int input_ref;

	int init_inputstream_type;
	int init_inputstream_nat;
	int init_inputstream_ref;

	int nextfloat_name;
	int nextfloat_type;
	int nextfloat_nat;
	int nextfloat_ref;

	int nextline_name;
	int nextline_type;
	int nextline_nat;
	int nextline_ref;

	int out_name;

	int source_file;
	int file;

	int hello_utf8;
	int hello_string;

	int dlang_main;
};
// ------------------------- DEFINITION FUNCTIONS ----------------------
struct ConstantId const_id;
struct SemanticClass *semantic;
FILE *file;

// ------------------------- CHECK SEMANTIC ----------------------------
struct SemanticClass *initSemantic();
bool doSemantic(struct Program *);

// BUILD SEMANTIC TREE
struct SemanticType checkSemanticExpr(struct Expression *, struct VarTable *);
struct SemanticType getSemanticType(enum BasicType, struct Dimention *);
bool checkSemanticStmList(struct StatementList *, struct VarTable *);
bool checkSemanticVarList(struct Variable *, struct VarTable *);
struct MethodTableConstant *checkSemanticFundef(struct FunctionDef *);
bool isEqualSemantic(struct SemanticType, struct SemanticType);

// PRINTERS
void printVarTable(FILE *, struct VarTable *);
void printMethodTable(FILE *);
void printConstTable(FILE *);
void printConst(FILE *, struct SemanticConstant *);

// VAR TABLE
void addToVarTable(struct Variable *, struct VarTable *);
struct VarTableConst *findVar(char *, struct VarTable *);

// METHOD TABLE
struct MethodTableConstant *addToMethodTable(struct FunctionDecl *);
struct MethodTableConstant *findMethod(char *);

// CONST TABLE
void addConstFunction();
void createConstTable(struct Program *);
int addToConstTable(enum SemanticConstantType, void *, void *);
void addStmListToConstTable(struct StatementList *);
void addVarsToConstTable(struct Variable *, enum Flag);
void addFuncDeclToConstTable(struct FunctionDecl *);
void addExprToConstTable(struct Expression *expr);
int findConstant(enum SemanticConstantType, void *, void *);

// GET VALUE JAVA RULE DESSCRIPTOR
char *getStrFromType(enum BasicType);
char *getTypeName(enum BasicType);
char *getVarDescriptor(enum BasicType, struct Dimention *);
char *getFuncDescriptor(enum BasicType, struct Dimention *, struct Variable *);

// TRANSFORM TREE
void addPosfixAssExpr(struct Expression *);
void addCastExpr(struct Expression *, struct SemanticType, enum Direction);
struct Expression *castExpr(struct Expression *, struct SemanticType);
#endif
