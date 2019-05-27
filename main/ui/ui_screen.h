#ifndef _UI_SCREEN_H
#define _UI_SCREEN_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "graphics.h"
#include "list.h"

#include "ui/ui_event.h"
#include "ui/ui_status.h"

#define UI_SCREEN_TITLE_HEIGHT 20

typedef struct ui_screen {
	char * title;
	graphics_color_t bg_color;
	list_t * bg;
	list_t * fg;

	void * list; // HACK: this is a void * instead of the ui_item_t * it should be

	void (*go_back)(struct ui_screen * screen);
	bool back_focused;

	void (*update)(struct ui_screen * screen);
	void (*draw)(struct ui_screen * screen);

	bool force_redraw;
} ui_screen_t;

void ui_screen_process_event(ui_screen_t * screen, ui_event_t * event);
void ui_screen_update(ui_screen_t * screen);
void ui_screen_draw(ui_screen_t * screen);

ui_screen_t * ui_screen_new(char * title);
void ui_screen_free(ui_screen_t * screen);

#endif