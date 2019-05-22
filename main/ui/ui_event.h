#ifndef _UI_EVENT_H
#define _UI_EVENT_H

#include <stdint.h>

// must match INPUT_* in input.h!
#define UI_BUTTON_SELECT (1 << 0)
#define UI_BUTTON_DOWN (1 << 1)
#define UI_BUTTON_RIGHT (1 << 2)
#define UI_BUTTON_UP (1 << 3)
#define UI_BUTTON_LEFT (1 << 4)
#define UI_BUTTON_A (1 << 5)
#define UI_BUTTON_B (1 << 6)

#define UI_EVENT_TYPE_BUTTON_DOWN 0
#define UI_EVENT_TYPE_BUTTON_UP 1

#define UI_EVENT_QUEUE_SIZE 16

typedef struct {
	uint8_t event_type;
	uint8_t event_data;
} ui_event_t;

#endif