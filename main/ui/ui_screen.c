#include "ui/ui_screen.h"

#include "ui/ui_item.h"

#include "ui/ui_list.h"

#include "graphics.h"
#include "font/font.h"

static void ui_screen_shift_focus(ui_screen_t * screen, int8_t direction) {
	if (screen->back_focused) {
		screen->back_focused = false;
	}

	if (screen->list) {
		// there's a list absorbing all of the focus events
		if (!ui_list_shift_focus(screen, (ui_item_t *) screen->list, direction)) {
			// it didn't move, is there a back button?
			if (screen->go_back) {
				// there is, focus on that
				screen->back_focused = true;

				// also unfocus the list
				ui_list_metadata_t * list_data = (ui_list_metadata_t *) ((ui_item_t *) screen->list)->metadata;
				list_data->_current_focus_item = NULL;
				list_data->_current_focus = 0;

				screen->force_redraw = true;
			}
		}
		return;
	}

	// find what's currently focused and move to the next/previous item
	list_item_t * list_entry;
	bool found_focused = false;

	if (screen->fg) {
		list_entry = screen->fg->head;
		while (list_entry) {
			ui_item_t * item = (ui_item_t *) list_entry->value;
			if (item->focus) {
				// this is the focused thing
				// move the focus to the next thing
				item->focus = false;
				ui_item_t * new_focus_target;
				if (direction == 1) {
					if (list_entry->next) {
						new_focus_target = (ui_item_t *) (list_entry->next)->value;
					} else {
						new_focus_target = (ui_item_t *) (screen->fg->head)->value;
					}
				} else { // if (direction == -1) {
					if (list_entry->prev) {
						new_focus_target = (ui_item_t *) (list_entry->prev)->value;
					} else {
						// find the last item
						while (list_entry) {
							if (list_entry->next) {
								list_entry = list_entry->next;
							} else {
								break;
							}
						}
						new_focus_target = (ui_item_t *) list_entry->value;
					}
				}
				new_focus_target->focus = true;
				found_focused = true;
				break;
			}
			list_entry = list_entry->next;
		}

		if (!found_focused) {
			// nothing is focused, choose the first item
			if (screen->fg && screen->fg->count > 0) {
				ui_item_t * item = (ui_item_t *) (screen->fg->head)->value;
				item->focus = true;
			} else if (screen->go_back) {
				screen->back_focused = true;
			}
		}

		screen->force_redraw = true;
	}
}

static void ui_screen_select(ui_screen_t * screen) {
	if (screen->back_focused) {
		screen->back_focused = false;
		screen->go_back(screen);
		return;
	}

	if (screen->list) {
		// there's a list absorbing all of the focus events
		ui_list_select(screen, (ui_item_t *) screen->list);
		return;
	}

	// find what's currently focused
	list_item_t * list_entry;

	if (screen->fg) {
		list_entry = screen->fg->head;
		while (list_entry) {
			ui_item_t * item = (ui_item_t *) list_entry->value;
			if (item->focus) {
				// this is the focused thing
				// activate it
				item->activate(item, screen);
				return;
			}
			list_entry = list_entry->next;
		}
	}
}

void ui_screen_process_event(ui_screen_t * screen, ui_event_t * event) {
	if (event->event_type == UI_EVENT_TYPE_BUTTON_DOWN) {
		// ignore
	} else if (event->event_type == UI_EVENT_TYPE_BUTTON_UP) {
		if (event->event_data == UI_BUTTON_RIGHT) {
			ui_screen_shift_focus(screen, 1);
		} else if (event->event_data == UI_BUTTON_LEFT) {
			ui_screen_shift_focus(screen, -1);
		} else if (event->event_data == UI_BUTTON_SELECT) {
			ui_screen_select(screen);
		}
	} else if (event->event_type == UI_EVENT_TYPE_SCROLL) {
		if (event->event_data == UI_SCROLL_CW) {
			ui_screen_shift_focus(screen, 1);
		} else if (event->event_data == UI_SCROLL_CCW) {
			ui_screen_shift_focus(screen, -1);
		}
	}
}

void ui_screen_update(ui_screen_t * screen) {
	if (screen->list) {
		ui_item_t * list = (ui_item_t *) screen->list;
		list->update(list, screen);
	}

	if (screen->update) {
		screen->update(screen);
	}
}

void ui_screen_draw(ui_screen_t * screen) {
	list_item_t * list_entry;

	graphics_fill_rect(0, 0, GRAPHICS_WIDTH, GRAPHICS_HEIGHT, screen->bg_color);

	if (screen->list) {
		ui_item_t * list = (ui_item_t *) screen->list;
		list->draw(list, screen);
	}

	if (screen->bg) {
		list_entry = screen->bg->head;
		while (list_entry) {
			ui_item_t * item = (ui_item_t *) list_entry->value;
			if (item->visible) {
				item->draw(item, screen);
			}
			list_entry = list_entry->next;
		}
	}

	bool show_back = (screen->go_back != NULL);

	if (screen->title) {
		graphics_fill_rect(0, UI_STATUS_HEIGHT, GRAPHICS_WIDTH, UI_SCREEN_TITLE_HEIGHT, GRAPHICS_COLOR_DARK_GRAY);
		graphics_draw_text(screen->title, 4 + (show_back ? 21 : 0), UI_STATUS_HEIGHT + (UI_SCREEN_TITLE_HEIGHT - 11) / 2, &font_source_sans_16, GRAPHICS_COLOR_WHITE);
		if (show_back) {
			graphics_draw_icon(4, UI_STATUS_HEIGHT + 4, &icon_back_title);
			graphics_fill_rect(20, UI_STATUS_HEIGHT, 1, UI_SCREEN_TITLE_HEIGHT, GRAPHICS_COLOR_WHITE);

			if (screen->back_focused) {
				graphics_draw_rect(0, UI_STATUS_HEIGHT, 20, 20, 2, GRAPHICS_COLOR_BLUE);
			}
		}
	}

	if (screen->draw) {
		screen->draw(screen);
	}

	if (screen->fg) {
		list_entry = screen->fg->head;
		while (list_entry) {
			ui_item_t * item = (ui_item_t *) list_entry->value;
			if (item->visible) {
				item->draw(item, screen);
			}
			list_entry = list_entry->next;
		}
	}

	ui_status_draw();
}

ui_screen_t * ui_screen_new(char * title) {
	ui_screen_t * screen = calloc(1, sizeof(ui_screen_t));
	if (title) {
		screen->title = strdup(title);
	}
	screen->update = NULL;
	screen->draw = NULL;
	screen->deinit = NULL;
	screen->bg_color = GRAPHICS_COLOR_WHITE;
	screen->bg = list_new((void (*)(void *)) ui_item_free);
	screen->fg = list_new((void (*)(void *)) ui_item_free);
	return screen;
}

void ui_screen_free(ui_screen_t * screen) {
	if (screen->title) {
		free(screen->title);
	}
	list_free(screen->bg);
	list_free(screen->fg);
	free(screen);
}