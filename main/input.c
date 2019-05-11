#include "input.h"

#include "ui.h"

const char * INPUT_TAG = "input";

uint8_t input_last_state = 0xFF;

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
	ui_handle_event(event);
}

void input_step() {
	uint8_t state = mcp23008_read_register(MCP23008_GPIO);
	uint8_t changes = input_last_state ^ state;

	if (changes & INPUT_SELECT) { input_handle_change(state, INPUT_SELECT); }
	if (changes & INPUT_DOWN) { input_handle_change(state, INPUT_DOWN); }
	if (changes & INPUT_RIGHT) { input_handle_change(state, INPUT_RIGHT); }
	if (changes & INPUT_UP) { input_handle_change(state, INPUT_UP); }
	if (changes & INPUT_LEFT) { input_handle_change(state, INPUT_LEFT); }
	// TODO: handle these properly
	// if (changes & INPUT_A) { input_handle_change(state, INPUT_A); }
	// if (changes & INPUT_B) { input_handle_change(state, INPUT_B); }

	input_last_state = state;
}