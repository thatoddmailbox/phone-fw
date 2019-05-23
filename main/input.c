#include "input.h"

#include "ui/ui.h"

const char * INPUT_TAG = "input";

uint8_t input_last_state = 0xFF;
uint8_t input_last_tick = 0;

void input_init() {

}

static void input_handle_change(uint8_t current_state, uint8_t button) {
	uint8_t event_type = UI_EVENT_TYPE_BUTTON_DOWN;
	if (current_state & button) {
		event_type = UI_EVENT_TYPE_BUTTON_UP;
	}

	ui_event_t event = {
		.event_type = event_type,
		.event_data = button
	};
	ui_push_event(event);
}

static void input_handle_scroll(uint8_t direction) {
	ui_event_t event = {
		.event_type = UI_EVENT_TYPE_SCROLL,
		.event_data = direction
	};
	ui_push_event(event);
}

void input_step() {
	uint8_t state = mcp23008_read_register(MCP23008_GPIO);
	uint8_t changes = input_last_state ^ state;

	if (changes & INPUT_SELECT) { input_handle_change(state, INPUT_SELECT); }
	if (changes & INPUT_DOWN) { input_handle_change(state, INPUT_DOWN); }
	if (changes & INPUT_RIGHT) { input_handle_change(state, INPUT_RIGHT); }
	if (changes & INPUT_UP) { input_handle_change(state, INPUT_UP); }
	if (changes & INPUT_LEFT) { input_handle_change(state, INPUT_LEFT); }

	if ((changes & INPUT_A) || (changes & INPUT_B)) {
		if (input_last_tick != 0) {
			if (input_last_tick == INPUT_A && (changes & INPUT_B)) {
				// went A -> B, so clockwise
				input_handle_scroll(INPUT_SCROLL_CW);
			} else if (input_last_tick == INPUT_B && (changes & INPUT_A)) {
				// went B -> A, so counterclockwise
				input_handle_scroll(INPUT_SCROLL_CCW);
			}
			input_last_tick = 0;
		} else{
			if (changes & INPUT_A) {
				input_last_tick = INPUT_A;
			} else {
				input_last_tick = INPUT_B;
			}
		}
	}

	input_last_state = state;
}