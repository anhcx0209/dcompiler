#ifndef _BYTE_CODE_STRUCT_H_
#define _BYTE_CODE_STRUCT_H_

#include <stdint.h>

struct ByteElement {
	uint8_t data;
	struct ByteElement* next;
};

struct ByteList {
	struct ByteElement* first;
	struct ByteElement* last;
	int size;
};

// ----------------- GLOBAL VARIABLE -----------------------
struct ByteList *main_code;

void generateByteCode();
struct ByteList *createByteList();

struct ByteList* generateCodeConstTable();
struct ByteList *generateCodeFieldTable();


void appendByte(struct ByteList *list, uint8_t data);
void append2Bytes(struct ByteList *list, uint16_t data);
void append4Bytes(struct ByteList *list, uint32_t data);
void appendList(struct ByteList *list1, struct ByteList *list2);


void fprintList(struct ByteList *list, FILE *pFile);
void printList(struct ByteList *list);

#endif // BYTE_CODE_STRUCT_H