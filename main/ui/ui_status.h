#ifndef _UI_STATUS_H
#define _UI_STATUS_H

#include <stdatomic.h>
#include <time.h>

#include "graphics.h"

#include "device/m26.h"

#include "watcher/m26_watcher.h"

#define UI_STATUS_HEIGHT 10

bool ui_status_dirty();
void ui_status_draw();

#endif