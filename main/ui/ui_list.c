#include "ui/ui_list.h"

static void ui_list_update(ui_item_t * list, ui_screen_t * screen) {

}

static void ui_list_draw_item(ui_item_t * list, ui_list_item_t * list_item, bool focus, graphics_point_t x, graphics_point_t y) {
	if (list_item->icon) {
		graphics_draw_icon(x, y, list_item->icon);
	}

	graphics_draw_rect(x, y, UI_LIST_ITEM_HEIGHT, UI_LIST_ITEM_HEIGHT, 2, (focus ? GRAPHICS_COLOR_BLUE : GRAPHICS_COLOR_BLACK));

	if (list_item->label) {
		graphics_draw_text(list_item->label, x + UI_LIST_ITEM_HEIGHT + 10, y + ((UI_LIST_ITEM_HEIGHT - 11) / 2) + 1, &font_source_sans_16, GRAPHICS_COLOR_BLACK);
	}
}

static void ui_list_draw(ui_item_t * list, ui_screen_t * screen) {
	ui_list_metadata_t * metadata = (ui_list_metadata_t *) list->metadata;

	if (metadata->items->count != 0) {
		list_item_t * focus_item = metadata->_current_focus_item;
		if (!focus_item) {
			focus_item = metadata->items->head;
		}

		graphics_point_t item_x = 10;
		graphics_point_t item_y = 10;

		if (focus_item->prev) {
			// we're not looking at the first item
			// so, our current position should be centered rather than top
			item_y = ((list->h - UI_LIST_ITEM_HEIGHT) / 2);

			// draw the previous item
			ui_list_item_t * prev_item = (ui_list_item_t *) focus_item->prev->value;
			ui_list_draw_item(list, prev_item, false, list->x + item_x, list->y + item_y - UI_LIST_ITEM_TOTAL_HEIGHT);
		}

		list_item_t * current_item = focus_item;

		for (size_t i = 0; i < 3; i++) {
			bool focus = (metadata->_current_focus_item == current_item);

			ui_list_item_t * item = (ui_list_item_t *) current_item->value;

			ui_list_draw_item(list, item, focus, list->x + item_x, list->y + item_y);

			item_y += UI_LIST_ITEM_TOTAL_HEIGHT;

			if (current_item->next) {
				current_item = current_item->next;
			} else {
				break;
			}
		}
	} else {
		graphics_draw_text("List is empty", list->x + 5, list->y + 5, &font_source_sans_16, GRAPHICS_COLOR_BLACK);
	}
}

static void ui_list_free(ui_item_t * list) {
	if (list->metadata) {
		ui_list_metadata_t * metadata = (ui_list_metadata_t *) list->metadata;
		free(metadata);
	}
}

bool ui_list_shift_focus(ui_screen_t * screen, ui_item_t * list, int8_t direction) {
	ui_list_metadata_t * list_data = (ui_list_metadata_t *) list->metadata;

	if (list_data->_current_focus_item) {
		if (direction == 1) {
			if (list_data->_current_focus_item->next) {
				list_data->_current_focus_item = list_data->_current_focus_item->next;
				list_data->_current_focus += 1;
				screen->force_redraw = true;
				return true;
			} else {
				return false;
			}
		} else if (direction == -1) {
			if (list_data->_current_focus_item->prev) {
				list_data->_current_focus_item = list_data->_current_focus_item->prev;
				list_data->_current_focus -= 1;
				screen->force_redraw = true;
				return true;
			} else {
				return false;
			}
		}
	} else {
		if (list_data->items->count != 0) {
			list_data->_current_focus_item = list_data->items->head;
			list_data->_current_focus = 0;
			screen->force_redraw = true;
			return true;
		} else {
			return false;
		}
	}

	return false;
}

void ui_list_select(ui_screen_t * screen, ui_item_t * list) {
	ui_list_metadata_t * list_data = (ui_list_metadata_t *) list->metadata;

	if (list_data->_current_focus_item) {
		ui_list_item_t * focused_item = (ui_list_item_t *) list_data->_current_focus_item->value;
		focused_item->activate(focused_item, list, screen);
	}
}

ui_item_t * ui_list_new(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
	ui_item_t * list = ui_item_new();

	list->x = x;
	list->y = y;
	list->w = w;
	list->h = h;
	list->can_focus = false;
	list->activate = NULL;
	list->update = &ui_list_update;
	list->draw = &ui_list_draw;
	list->free = &ui_list_free;

	ui_list_metadata_t * metadata = calloc(1, sizeof(ui_list_metadata_t));
	metadata->items = list_new((void (*)(void *)) &ui_list_item_free);
	list->metadata = metadata;

	return list;
}

ui_list_item_t * ui_list_item_new(const char * label, const icon_t * icon, void (*activate)(ui_list_item_t * list_item, ui_item_t * list, ui_screen_t * screen)) {
	ui_list_item_t * list_item = calloc(1, sizeof(ui_list_item_t));

	list_item->label = label;
	list_item->icon = icon;
	list_item->activate = activate;

	return list_item;
}

void ui_list_item_free(ui_list_item_t * item) {
	free(item);
}
