#include "input.h"

#include "ui/ui.h"

const char * INPUT_TAG = "input";

uint8_t input_last_state = 0xFF;
uint8_t input_last_state_b = 0xFF;
uint8_t input_last_tick = 0;

void input_init() {
	// all inputs
	pcal6416_write_register(PCAL6416_CONFIG_0, 0b1111111);

	// enable pullups
	pcal6416_write_register(PCAL6416_PULL_ENABLE_0, 0b1111111);
	pcal6416_write_register(PCAL6416_PULL_SELECT_0, 0b1111111);

	// enable interrupts whenever a pin flips
	// pcal6416_write_register(PCAL6416_GPINTEN, 0b1111111);
	// pcal6416_write_register(PCAL6416_INTCON, 0b00000000);

	expio_set_direction(9 - 8, EXPIO_INPUT);
	expio_set_direction(10 - 8, EXPIO_INPUT);
	expio_set_direction(11 - 8, EXPIO_INPUT);
	expio_set_pullup(9 - 8, true);
	expio_set_pullup(10 - 8, true);
	expio_set_pullup(11 - 8, true);
}

static void input_handle_change(uint16_t current_state, uint16_t button) {
	uint8_t event_type = UI_EVENT_TYPE_BUTTON_DOWN;
	if (current_state & button) {
		event_type = UI_EVENT_TYPE_BUTTON_UP;
	}

	uint16_t mapped_button = button;

#if HW_VERSION != EVAL_HW
	// buttons must be remapped on non-eval boards
	if (button == INPUT_DOWN) {
		mapped_button = INPUT_RIGHT;
	} else if (button == INPUT_RIGHT) {
		mapped_button = INPUT_UP;
	} else if (button == INPUT_UP) {
		mapped_button = INPUT_LEFT;
	} else if (button == INPUT_LEFT) {
		mapped_button = INPUT_DOWN;
	}
#endif

	ui_event_t event = {
		.event_type = event_type,
		.event_data = mapped_button
	};
	ui_push_event(event);
}

static void input_handle_scroll(uint16_t direction) {
	ui_event_t event = {
		.event_type = UI_EVENT_TYPE_SCROLL,
		.event_data = direction
	};
	ui_push_event(event);
}

void input_step() {
#if HW_VERSION == EVAL_HW
	uint8_t state = mcp23008_read_register(MCP23008_GPIO);
#else
	uint8_t state = pcal6416_read_register(PCAL6416_INPUT_0);
#endif
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

#if HW_VERSION != EVAL_HW
	uint16_t state_b = pcal6416_read_register(PCAL6416_INPUT_1);
	uint8_t changes_b = input_last_state_b ^ state_b;

	if (changes_b & (INPUT_LOCK >> 8)) { input_handle_change(state_b << 8, INPUT_LOCK); }
	if (changes_b & (INPUT_VOL_UP >> 8)) { input_handle_change(state_b << 8, INPUT_VOL_UP); }
	if (changes_b & (INPUT_VOL_DOWN >> 8)) { input_handle_change(state_b << 8, INPUT_VOL_DOWN); }

	input_last_state_b = state_b;
#endif

	input_last_state = state;
}