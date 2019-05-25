#include "ui/ui_screen.h"

#include "ui/ui_item.h"

#include "graphics.h"
#include "font/font.h"

static void ui_screen_shift_focus(ui_screen_t * screen, int8_t direction) {
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
			ui_item_t * item = (ui_item_t *) (screen->fg->head)->value;
			item->focus = true;
		}

		screen->force_redraw = true;
	}
}

static void ui_screen_select(ui_screen_t * screen) {
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
	if (screen->update) {
		screen->update(screen);
	}
}

void ui_screen_draw(ui_screen_t * screen) {
	list_item_t * list_entry;

	graphics_fill_rect(0, 0, GRAPHICS_WIDTH, GRAPHICS_HEIGHT, screen->bg_color);

	bool show_back = true;

	if (screen->title) {
		graphics_fill_rect(0, 0, GRAPHICS_WIDTH, 20, GRAPHICS_COLOR_DARK_GRAY);
		graphics_draw_text(screen->title, 4 + (show_back ? 21 : 0), (20 - 11) / 2, &font_source_sans_16, GRAPHICS_COLOR_WHITE);
		if (show_back) {
			graphics_draw_icon(4, 4, &icon_back_title);
			graphics_fill_rect(20, 0, 1, 20, GRAPHICS_COLOR_WHITE);
		}
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
}

ui_screen_t * ui_screen_new(char * title) {
	ui_screen_t * screen = calloc(1, sizeof(ui_screen_t));
	if (title) {
		screen->title = strdup(title);
	}
	screen->update = NULL;
	screen->draw = NULL;
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