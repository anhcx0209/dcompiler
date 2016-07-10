#ifndef _BYTE_CODE_H_
#define _BYTE_CODE_H_

#include <stdbool.h>
#include "bytecode_struct.h"

void generateByteCode() {
	main_code = createByteList();

	// MAGIC NUMBER
	append4Bytes(main_code, 0xCAFEBABE);

	// MINOR AND MAJOR VERSION 
	append4Bytes(main_code, 0x0003002d);
	struct ByteList* ct_code = generateCodeConstTable();


	// PRINT LIST
	printList(main_code);
}


struct ByteList* generateCodeConstTable() {
	struct ByteList *code = createByteList();
	// empty const table
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