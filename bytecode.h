#ifndef _BYTE_CODE_H_
#define _BYTE_CODE_H_

#include <stdbool.h>
#include "bytecode_struct.h"

void generateByteCode() {
	FILE *output = fopen("d.class", "w");

	main_code = createByteList();

	// MAGIC NUMBER
	append4Bytes(main_code, 0xCAFEBABE);

	// MINOR AND MAJOR VERSION 
	append4Bytes(main_code, 0x0003002d);
	
	// CONSTANT_POOL_COUNT AND CONSTANT_POOL [CONSTANT_POOL_COUNT - 1]	
	struct ByteList* ct_code = generateCodeConstTable();

	// PRINT LIST
	//fprintList(main_code, output);
	//printList(main_code);

	fclose(output);
}


struct ByteList* generateCodeConstTable() {
	struct ByteList* code = createByteList();	
	// get number of constant and put to list
	uint16_t cnt = (uint16_t) (semantic->const_table->size + 1);
	append2Bytes(code, cnt);
	uint16_t l;
	uint32_t val;
	int i;
	// put every constant to list
	for (struct SemanticConstant* c = semantic->const_table->first; c != NULL; c = c->next) {
		switch (c->type) {
			case CONSTANT_Utf8:
				appendByte(code, 0x1);
				l = (uint16_t)strlen(c->utf8_val);
				append2Bytes(code, l);
				for (i = 0; i < l; i++)
					appendByte(code, (uint8_t)c->utf8_val[i]);
				break;
			case CONSTANT_Integer:
				appendByte(code, 0x3);
				append4Bytes(code, (uint32_t)c->int_val);
				break;
			case CONSTANT_Float:
				appendByte(code, 0x4);
				val = *(uint32_t*)&(c->float_val);
				append4Bytes(code, val);	
				break;
			case CONSTANT_Class:
				appendByte(code, 0x7);
				append2Bytes(code, (uint16_t)c->link1);	
				break;			
			case CONSTANT_String:
				appendByte(code, 0x8);
				append2Bytes(code, (uint16_t)c->link1);	
				break;
			case CONSTANT_Fieldref:
				appendByte(code, 0x9);
				append2Bytes(code, (uint16_t)c->link1);	
				append2Bytes(code, (uint16_t)c->link2);	
				break;
			case CONSTANT_Methodref:
				appendByte(code, 0xA);
				append2Bytes(code, (uint16_t)c->link1);	
				append2Bytes(code, (uint16_t)c->link2);	
				break;
			case CONSTANT_NameAndType:
				appendByte(code, 0xC);
				append2Bytes(code, (uint16_t)c->link1);	
				append2Bytes(code, (uint16_t)c->link2);	
				break;
		}
	}
	return code;
}

struct ByteList *createByteList() {
	struct ByteList* code = (struct ByteList*) malloc(sizeof(struct ByteList));
	code->first = code->last = NULL;
	code->size = 0;
	
	return code;
}

void appendByte(struct ByteList *list, uint8_t data) {
	// create new byte element
	struct ByteElement *e = (struct ByteElement *)malloc(sizeof(struct ByteElement));
	e->data = data;
	e->next = NULL;

	// add to list
	if (list->first == NULL) {
		list->first = list->last = e;
	} else {
		list->last->next = e;
		list->last = e;
	}
	list->size++;
}

void append2Bytes(struct ByteList *list, uint16_t data) {
	appendByte(list, (uint8_t) ((data >> 8) & 0xFF) );
	appendByte(list, (uint8_t) (data & 0xFF));
}

void append4Bytes(struct ByteList *list, uint32_t data) {
	append2Bytes(list, (uint16_t) ((data >> 16) & 0xFFFF));
	append2Bytes(list, (uint16_t) (data & 0xFFFF));
}

void fprintList(struct ByteList *list, FILE *pFile) {
	if (list != NULL) {
		for (struct ByteElement *i = list->first; i != NULL; i = i->next) {
			fwrite(&i->data, 1, 1, pFile);
		}
	}
}

void printList(struct ByteList *list) {
	if (list != NULL) {
		for (struct ByteElement *i = list->first; i != NULL; i = i->next) {
			printf("%02X\n", i->data);
		}
	}
}

#endif // BYTE_CODE_H