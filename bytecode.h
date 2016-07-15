#ifndef _BYTE_CODE_H_
#define _BYTE_CODE_H_

#include <stdbool.h>
#include "bytecode_struct.h"

void generateByteCode() {
	FILE *output = fopen("DLang.class", "w");

	main_code = createByteList();

	// MAGIC NUMBER
	append4Bytes(main_code, 0xCAFEBABE);

	// MINOR AND MAJOR VERSION 
	append4Bytes(main_code, 0x0003002d);
	
	// CONSTANT_POOL_COUNT AND CONSTANT_POOL [CONSTANT_POOL_COUNT - 1]	
	struct ByteList* ct_code = generateCodeConstTable();
	appendList(main_code, ct_code);

	// ACCESS_FLAGS ACC_SUPER | ACC_PUBLIC
	append2Bytes(main_code, 0x21);

	// THIS_CLASS 
	append2Bytes(main_code, (uint16_t) const_id.this_class);

	// SUPER_CLASS
	append2Bytes(main_code, (uint16_t) const_id.object_class);	

	// INTERFACE_COUNT
	append2Bytes(main_code, 0x0);

	// FIELDS_COUNT AND FIELDS[FIELDS_COUNT]
	struct ByteList *ft_code = generateCodeFieldTable();
	appendList(main_code, ft_code);

	// PRINT LIST
	fprintList(main_code, output);
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

struct ByteList *generateCodeFieldTable() {
	struct ByteList* code = createByteList();

	uint16_t cnt = (uint16_t) (semantic->field_table->size);
	// 1 - in field for reading from console
	append2Bytes(code, cnt + 1);	

	for (struct VarTableConst* var = semantic->field_table->first; var != NULL; var = var->next){		
		append2Bytes(code, 0x8); // access_flags = static
		append2Bytes(code, (uint16_t)var->name_index);
		append2Bytes(code, (uint16_t)var->desc_index);
		append2Bytes(code, 0x0); // attributes_count
	}
	append2Bytes(code, 0x8);
	append2Bytes(code, const_id.input_name);
	append2Bytes(code, const_id.input_type);
	append2Bytes(code, 0x0);

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

void appendList(struct ByteList *list1, struct ByteList *list2)	{
	if (list2 != NULL) 
		for (struct ByteElement *e = list2->first; e != NULL; e = e->next) 
			appendByte(list1, e->data);
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