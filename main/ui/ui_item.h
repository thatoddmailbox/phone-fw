#ifndef _UI_ITEM_H
#define _UI_ITEM_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct ui_item {
	uint8_t x, y;
	uint8_t w, h;

	bool can_focus;
	bool focus;

	void * metadata;

	void (*activate)(struct ui_item * item); // , struct ui_screen * screen);
	void (*update)(struct ui_item * item); // , struct ui_screen * screen);
	void (*draw)(struct ui_item * item); // , struct ui_screen * screen);
	void (*free)(struct ui_item * item);
} ui_item_t;

void ui_item_free(ui_item_t * item);

#endif