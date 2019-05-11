#include "list.h"

list_t * list_new(void (*free)(void * item)) {
	list_t * l = malloc(sizeof(list_t));
	l->free = free;
	l->count = 0;
	l->head = NULL;
	return l;
}

void list_add(list_t * l, void * item) {
	l->count++;
	
	list_item_t * new_item = malloc(sizeof(list_item_t));
	new_item->value = item;
	new_item->next = NULL;
	new_item->prev = NULL;
	if (l->head) {
		list_item_t * current_item = l->head;
		list_item_t * previous_item = NULL;
		while (current_item) {
			previous_item = current_item;
			current_item = current_item->next;
		}
		// previous_item is now the second-to-last item in the list
		previous_item->next = new_item;
		new_item->prev = previous_item;
	} else {
		l->head = new_item;
	}
}

void list_free(list_t * l) {
	list_item_t * i = l->head;
	while (i) {
		l->free(i->value); // free the actual value
		list_item_t * next_item = i->next; // save the pointer
		free(i); // free the item struct
		i = next_item; // continue
	}
	free(l);
}