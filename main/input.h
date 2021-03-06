#ifndef _INPUT_H
#define _INPUT_H

#include "device/mcp23008.h"
#include "device/pcal6416.h"

// must match UI_BUTTON_* in ui/ui_event.h!
#define INPUT_SELECT (1 << 0)
#define INPUT_DOWN (1 << 1)
#define INPUT_RIGHT (1 << 2)
#define INPUT_UP (1 << 3)
#define INPUT_LEFT (1 << 4)
#define INPUT_A (1 << 5)
#define INPUT_B (1 << 6)
#define INPUT_SCROLL_CW INPUT_A
#define INPUT_SCROLL_CCW INPUT_B
#define INPUT_BATT_STAT (1 << 7)
#define INPUT_LOCK (1 << 9)
#define INPUT_VOL_UP (1 << 10)
#define INPUT_VOL_DOWN (1 << 11)

void input_init();
void input_step();

#endif