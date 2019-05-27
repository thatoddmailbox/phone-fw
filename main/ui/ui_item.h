#ifndef _UI_ITEM_H
#define _UI_ITEM_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "ui/ui_screen.h"

typedef struct ui_item {
	graphics_point_t x, y;
	graphics_point_t w, h;

	bool can_focus;
	bool focus;
	bool visible;

	void * metadata;

	void (*activate)(struct ui_item * item, ui_screen_t * screen);
	void (*update)(struct ui_item * item, ui_screen_t * screen);
	void (*draw)(struct ui_item * item, ui_screen_t * screen);
	void (*free)(struct ui_item * item);
} ui_item_t;

ui_item_t * ui_item_new();
void ui_item_free(ui_item_t * item);

#endif