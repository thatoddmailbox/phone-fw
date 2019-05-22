#include "ui/ui_button.h"

static void ui_button_activate(struct ui_item * item, ui_screen_t * screen) {
	((ui_button_metadata_t *) item->metadata)->click(item, screen);
}

static void ui_button_update(struct ui_item * item, ui_screen_t * screen) {

}

static void ui_button_draw(struct ui_item * item, ui_screen_t * screen) {
	ui_button_metadata_t * metadata = ((ui_button_metadata_t *) item->metadata);

	graphics_fill_rect(item->x, item->y, item->w, item->h, GRAPHICS_COLOR_GREEN);
	if (item->focus) {
		graphics_draw_rect(item->x, item->y, item->w, item->h, 2, GRAPHICS_COLOR_BLUE);
	} else {
		graphics_draw_rect(item->x, item->y, item->w, item->h, 2, GRAPHICS_COLOR_BLACK);
	}
	
	if (metadata->text) {
		graphics_draw_text(metadata->text, item->x + item->w + 10, item->y + ((item->h - 11) / 2) + 1, &font_source_sans_16, GRAPHICS_COLOR_BLACK);
	}
}

static void ui_button_free(struct ui_item * item) {
	if (item->metadata) {
		ui_button_metadata_t * metadata = (ui_button_metadata_t *) item->metadata;
		if (metadata->text) {
			free(metadata->text);
		}
		free(metadata);
	}
}

ui_item_t * ui_button_new(char * text, uint8_t x, uint8_t y, uint8_t w, uint8_t h, void (*click)(ui_item_t * button, ui_screen_t * screen)) {
	ui_item_t * item = ui_item_new();
	item->x = x;
	item->y = y;
	item->w = w;
	item->h = h;
	item->can_focus = true;
	item->activate = ui_button_activate;
	item->update = ui_button_update;
	item->draw = ui_button_draw;
	item->free = ui_button_free;

	ui_button_metadata_t * metadata = calloc(1, sizeof(ui_button_metadata_t));
	metadata->text = strdup(text);
	metadata->click = click;
	item->metadata = metadata;

	return item;
}