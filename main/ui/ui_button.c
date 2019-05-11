#include "ui_button.h"

static void ui_button_activate(struct ui_item * item) { // , struct ui_screen * screen);
	
}

static void ui_button_update(struct ui_item * item) { // , struct ui_screen * screen);
	
}

static void ui_button_draw(struct ui_item * item) { // , struct ui_screen * screen);
	graphics_fill_rect(item->x, item->y, item->w, item->h, GRAPHICS_COLOR_GREEN);
	if (item->focus) {
		graphics_draw_rect(item->x, item->y, item->w, item->h, GRAPHICS_COLOR_BLUE);
	}
}

ui_item_t * ui_button_new(char * text, uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
	ui_item_t * item = calloc(1, sizeof(ui_item_t));
	item->x = x;
	item->y = y;
	item->w = w;
	item->h = h;
	item->can_focus = true;
	item->activate = ui_button_activate;
	item->update = ui_button_update;
	item->draw = ui_button_draw;
	return item;
}