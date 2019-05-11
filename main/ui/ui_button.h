#ifndef _UI_BUTTON_H
#define _UI_BUTTON_H

#include <stdint.h>

#include "graphics.h"

#include "ui_item.h"

ui_item_t * ui_button_new(char * text, uint8_t x, uint8_t y, uint8_t w, uint8_t h);

#endif