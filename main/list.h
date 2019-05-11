#ifndef _LIST_H
#define _LIST_H

#include <stdint.h>
#include <stdlib.h>

typedef struct list_item {
	void * value;
	struct list_item * next;
	struct list_item * prev;
} list_item_t;

typedef struct list {
	list_item_t * head;
	uint32_t count;
	void (*free)(void * item);
} list_t;

list_t * list_new(void (*free)(void * item));
void list_add(list_t * l, void * value);
void list_free(list_t * l);

#endif