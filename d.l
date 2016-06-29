%{
#include "tree_struct.h"
#include "semantic.h"
#include "bytecode.h"
#include "d.tab.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tree_print.h"

extern int yyparse(void);
	
char * strbuf;
int line_number = 1;

int getInt(char *);
float getFloat(char *);
char getChar(char *);
void appendString(char*);
void appendChar(char);
%}

%option noyywrap
%option never-interactive

D				[0-9_]
L				[a-zA-Z_]
H				[a-fA-F0-9_]
E				[Ee][+-]?{D}+
IS				(L|u|U){1,2}		
FS				(f|F|L)				

%x nestcomment
%x string
%%


"writeln"		{ 
					printf("Found writeln function: %s\n", yytext); 
					return WRITELN;
				}
"readln"		{ 
					printf("Found readln function: %s\n", yytext); 
					return READLN;
				}
"float"			{ 
					printf("Found a keyword: %s\n", yytext); 
					return FLOAT;
				}				
"int"			{ 
					printf("Found a keyword: %s\n", yytext); 
					return INT;
				}
"char"			{ 
					printf("Found a keyword: %s\n", yytext); 
					return CHAR;
				}
"void"			{ 
					printf("Found a keyword: %s\n", yytext);
					return VOID;
				}
"if"			{ 
					printf("Found a keyword: %s\n", yytext);
					return IF;
				}
"else"			{ 
					printf("Found a keyword: %s\n", yytext);
					return ELSE;
				}
"break"			{ 
					printf("Found a keyword: %s\n", yytext);
					return BREAK;
				}
"continue"		{ 
					printf("Found a keyword: %s\n", yytext);
					return CONTINUE;
				}
"default"		{ 
					printf("Found a keyword: %s\n", yytext);
					return DEFAULT;
				}
"do"			{ 
					printf("Found a keyword: %s\n", yytext);
					return DO;
				}
"while"			{ 
					printf("Found a keyword: %s\n", yytext);
					return WHILE;
				}
"case"			{ 
					printf("Found a keyword: %s\n", yytext);
					return CASE;
				}
"switch"		{ 
					printf("Found a keyword: %s\n", yytext);
					return SWITCH;
				}
"for"			{ 
					printf("Found a keyword: %s\n", yytext);
					return FOR;
				}
"import"		{ 
					printf("Found a keyword: %s\n", yytext);
					return IMPORT;
				}
"const"			{ 
					printf("Found a keyword: %s\n", yytext); 
					return CONST;
				}
"return"		{ 
					printf("Found a keyword: %s\n", yytext);
					return RETURN;
				}
"+"				{
					printf("Found a token: %s\n", yytext);
					return '+';
				}
"-"				{
					printf("Found a token: %s\n", yytext);
					return '-';
				}
"*"				{
					printf("Found a token: %s\n", yytext);
					return '*';
				}
"/"				{
					printf("Found a token: %s\n", yytext);
					return '/';
				}
"~"				{
					printf("Found a token: %s\n", yytext);
					return '~';
				}	
"&&"			{
					printf("Found a token: %s\n", yytext);
					return AND_AND;
				}
"||"			{
					printf("Found a token: %s\n", yytext);
					return OR_OR;
				}
"!"				{
					printf("Found a token: %s\n", yytext);
					return '!';
				}

">"				{
					printf("Found a token: %s\n", yytext);
					return '>';
				}
"<"				{
					printf("Found a token: %s\n", yytext);
					return '<';
				}
">="			{
					printf("Found a token: %s\n", yytext);
					return GE;
				}
"<="			{
					printf("Found a token: %s\n", yytext);
					return LE;
				}
"=="			{
					printf("Found a token: %s\n", yytext);
					return EQ;
				}
"!="			{
					printf("Found a token: %s\n", yytext);
					return NEQ;
				}
"="				{
					printf("Found a token: %s\n", yytext);
					return '=';
				}
"+="			{
					printf("Found a token: %s\n", yytext);
					return ADD_ASS;
				}
"-="			{
					printf("Found a token: %s\n", yytext);
					return SUB_ASS;
				}
"*="			{
					printf("Found a token: %s\n", yytext);
					return MUL_ASS;
				}
"/="			{
					printf("Found a token: %s\n", yytext);
					return DIV_ASS;
				}
"~="			{
					printf("Found a token: %s\n", yytext);
					return CAT_ASS;
				}				
"--"			{
					printf("Found a token: %s\n", yytext);
					return SUB_SUB;
				}
"++"			{
					printf("Found a token: %s\n", yytext);
					return ADD_ADD;
				}
"."				{
					printf("Found a token: %s\n", yytext);
					return '.';
				}
"("				{
					printf("Found a token: %s\n", yytext);
					return '(';
				}
")"				{
					printf("Found a token: %s\n", yytext);
					return ')';
				}
"["				{
					printf("Found a token: %s\n", yytext);
					return '[';
				}
"]"				{
					printf("Found a token: %s\n", yytext);
					return ']';
				}
"{"				{
					printf("Found a token: %s\n", yytext);
					return '{';
				}
"}"				{
					printf("Found a token: %s\n", yytext);
					return '}';
				}
","				{
					printf("Found a token: %s\n", yytext);
					return ',';
				}
";"				{
					printf("Found a token: %s\n", yytext);
					return ';';
				}
":"				{
					printf("Found a token: %s\n", yytext);
					return ':';
				}
{L}({L}|{D})*				{ 
								printf("Found a IDENTIFIER %s\n", yytext);
								yylval.strval = (char*) malloc((strlen(yytext) + 1) * sizeof(char));
								strcpy(yylval.strval, yytext);
								return ID;
							}

0{D}+{IS}?					{ 
								printf("Found a INTEGER %d\n", getInt(yytext)); 
								yylval.ival = getInt(yytext);
								return INTVAL;
							}

{D}+{IS}?					{ 
								printf("Found a INTEGER %d\n", getInt(yytext)); 
								yylval.ival = getInt(yytext);
								return INTVAL;
							}
0[xX]{H}+{IS}?				{ 
								printf("Found a INTEGER %d\n", getInt(yytext)); 
								yylval.ival = getInt(yytext);
								return INTVAL;
							}

{D}+{E}{FS}?				{ 
								printf("Found a FLOAT %.3f\n", getFloat(yytext)); 
								yylval.fval = getFloat(yytext);
								return FLOATVAL;
							}
{D}*"."{D}*({E})?{FS}?		{ 
								printf("Found a FLOAT %.3f\n", getFloat(yytext)); 
								yylval.fval = getFloat(yytext);
								return FLOATVAL;
							}

"/*"(.|\n)*?"*/"			{ printf("Found a block comment\n"); }
"//".*						{ printf("Found a inline comment\n"); }

"/+"						{ BEGIN(nestcomment); }
<nestcomment>\n				{ ++line_number; }
<nestcomment>[^+]*			{ /* eat all char, but not plus */ }
<nestcomment>"+"+[^+/]*		{ /* eat all char, but not plus */ }
<nestcomment>"+"+"/"		{ printf("Found a nesting block comment\n"); BEGIN 0; }

[rR]\"(\\.|[^\\"])*\"		{ 
								printf("Found a wysiwyg string: %s\n", yytext); 
								yylval.strval = (char*) malloc((strlen(yytext) + 1) * sizeof(char));
								strcpy(yylval.strval, yytext);
								return STRINGVAL;
							}
\`(\\.|[^\\`])*\`			{ 
								printf("Found a wysiwyg string: %s\n", yytext); 
								yylval.strval = (char*) malloc((strlen(yytext) + 1) * sizeof(char));
								strcpy(yylval.strval, yytext);
								return STRINGVAL;
							}

\'(\\.|[^\\'])\'			{ 
								printf("Found a char literal: %c\n", getChar(yytext)); 
								yylval.cval = getChar(yytext);
								return CHARVAL;
							}

\"							{ BEGIN string; strbuf = NULL; }
<string>[^\\"\n]*			{ appendString(yytext); }
<string>\\n					{ appendChar('\n'); }
<string>\n					{ ++line_number; appendChar('\n'); }
<string>\\t					{ appendChar('\t'); }
<string>\\[^nt]				{ appendChar(yytext[1]); }
<string>\"					{ printf("Found string: %s\n", strbuf); BEGIN 0; yylval.strval = strbuf; return STRINGVAL; }

[ \t\r\f]					{ /* white space do nothing */ }
\n							{ ++line_number; }

.							{ /* ignore bac characters */ }
	
%%

int main(int argc, char *argv[])
{
	yyin=fopen("sort.d","r");

	yyparse();
	if (doSemantic(prog))
		generateByteCode();

	//printTree();
    //yylex();    
    return 0;
}

void appendString(char* str) {
	if (strbuf == NULL) {
		strbuf = (char*)malloc((strlen(str) + 1) * sizeof(char));
		strcpy(strbuf, str);
	} else {
		int newLength = strlen(strbuf) + strlen(str);

		char * save = (char *)malloc((strlen(strbuf) + 1) * sizeof(char));
		strcpy(save, strbuf);

		strbuf = (char*)malloc((newLength + 1) * sizeof(char));
		strcpy(strbuf, save);
		strcat(strbuf, str);

		free(save);
	}	
}

void appendChar(char c) {
	if (strbuf == NULL) {
		strbuf = (char*)malloc(2 * sizeof(char));
		strbuf[0] = c;
		strbuf[1] = '\0';
	} else {
		char * save = (char *)malloc((strlen(strbuf) + 1) * sizeof(char));
		strcpy(save, strbuf);

		int l = strlen(strbuf);
		strbuf = (char*)malloc((l + 2) * sizeof(char));
		strcpy(strbuf, save);
		strbuf[l] = c;
		strbuf[l + 1] = '\0';

		free(save);
	}
}

char * strupr(char * s) {
	int n = strlen(s);
	for (int i = 0; i < n; i++) 
		s[i] = toupper(s[i]);
	return s;
}

int getInt(char * str) {	
	strcpy(str, strupr(str));
	int i = 0, n = strlen(str) + 1;
	char * temp = (char*)malloc(n * sizeof(char));

	for (int j = 0; j < n; j++) {
		if (str[j] != '_' && str[j] != 'U' && str[j] != 'L') {
			temp[i] = str[j];
			i++;
		}
	}

	temp[i] = '\0';
	int a = (int)strtol(temp, NULL, 0);
	
	return a;
}

char getChar(char* str) {
	if (str[1] != '\\') {
		return str[1];
	} else {
		switch (str[2]) {
			case 'n':
				return '\n';
			case '\"':
				return '\"';
			case '\?':
				return '\?';
			case '\\':
				return '\\';
			case 't':
				return '\t';
			case '\'':
				return '\'';
			default: 
				return str[2];
		}
	}
}

float getFloat(char * str) {
	strcpy(str, strupr(str));
	int i = 0, n = strlen(str) + 1;
	char * temp = (char*)malloc(n * sizeof(char));

	for (int j = 0; j < n; j++) {
		if (str[j] != '_' && str[j] != 'F' && str[j] != 'L') {
			temp[i] = str[j];
			i++;
		}
	}

	temp[i] = '\0';
	float f = (float)strtof(temp, NULL);
	return f;
}