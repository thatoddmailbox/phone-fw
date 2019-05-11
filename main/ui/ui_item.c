#include "ui_item.h"

void ui_item_free(ui_item_t * item) {
	if (item->free) {
		item->free(item);
	}
	free(item);
}