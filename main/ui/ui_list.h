#ifndef _UI_LIST_H
#define _UI_LIST_H

#include <stddef.h>
#include <stdint.h>

#include "graphics.h"

#include "ui/ui_item.h"

#define UI_LIST_ITEM_HEIGHT 32
#define UI_LIST_ITEM_PADDING 10
#define UI_LIST_ITEM_TOTAL_HEIGHT (UI_LIST_ITEM_HEIGHT + UI_LIST_ITEM_PADDING)

typedef struct ui_list_item {
	const icon_t * icon;
	const char * label;
	void (*activate)(struct ui_list_item * list_item, ui_item_t * list, ui_screen_t * screen);
} ui_list_item_t;

typedef struct ui_list_metadata {
	list_item_t * _current_focus_item;
	uint32_t _current_focus;

	list_t * items;
} ui_list_metadata_t;

void ui_list_shift_focus(ui_screen_t * screen, ui_item_t * list, int8_t direction);
void ui_list_select(ui_screen_t * screen, ui_item_t * list);

ui_item_t * ui_list_new(uint8_t x, uint8_t y, uint8_t w, uint8_t h);

ui_list_item_t * ui_list_item_new(const char * label, const icon_t * icon, void (*activate)(ui_list_item_t * list_item, ui_item_t * list, ui_screen_t * screen));
void ui_list_item_free(ui_list_item_t * item);

#endif