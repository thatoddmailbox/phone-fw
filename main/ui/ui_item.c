#include "ui/ui_item.h"

ui_item_t * ui_item_new() {
	ui_item_t * item = calloc(1, sizeof(ui_item_t));
	item->visible = true;
	return item;
}

void ui_item_free(ui_item_t * item) {
	if (item->free) {
		item->free(item);
	}
	free(item);
}