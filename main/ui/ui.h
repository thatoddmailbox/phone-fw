#ifndef _UI_H
#define _UI_H

#include <stdint.h>

#include <esp_log.h>

#include "ui/ui_event.h"
#include "ui/ui_screen.h"
#include "ui/ui_status.h"

void ui_init();
void ui_set_screen(ui_screen_t * screen);
void ui_push_event(ui_event_t event);
void ui_step();

#endif