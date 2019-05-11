#ifndef _UI_BUTTON_H
#define _UI_BUTTON_H

#include <stdint.h>

#include "graphics.h"

#include "ui/ui_item.h"

typedef struct {
	char * text;
	void (*click)(ui_item_t * button, ui_screen_t * screen);
} ui_button_metadata_t;

ui_item_t * ui_button_new(char * text, uint8_t x, uint8_t y, uint8_t w, uint8_t h, void (*click)(ui_item_t * button, ui_screen_t * screen));

#endif