#include "ui/ui_status.h"

static void ui_status_draw_signal_bars(uint8_t x, uint8_t y, uint8_t amount) {
	if (amount >= 1) { graphics_fill_rect(x, y + 6, 2, 2, GRAPHICS_COLOR_WHITE); }
	if (amount >= 2) { graphics_fill_rect(x + 3, y + 4, 2, 4, GRAPHICS_COLOR_WHITE); }
	if (amount >= 3) { graphics_fill_rect(x + 6, y + 2, 2, 6, GRAPHICS_COLOR_WHITE); }
	if (amount >= 4) { graphics_fill_rect(x + 9, y, 2, 8, GRAPHICS_COLOR_WHITE); }
}

static void ui_status_draw_battery(uint8_t x, uint8_t y, uint8_t amount) {
	// top thingy
	graphics_fill_rect(x + 1, y, 3, 1, GRAPHICS_COLOR_WHITE);

	// main outline
	graphics_fill_rect(x, y + 1, 5, 1, GRAPHICS_COLOR_WHITE);
	graphics_fill_rect(x, y + 1, 1, 7, GRAPHICS_COLOR_WHITE);
	graphics_fill_rect(x + 4, y + 1, 1, 7, GRAPHICS_COLOR_WHITE);
	graphics_fill_rect(x, y + 7, 5, 1, GRAPHICS_COLOR_WHITE);

	// fill with level
	graphics_fill_rect(x + 1, y + 2 + (5 - amount), 3, amount, GRAPHICS_COLOR_WHITE);
}

bool ui_status_dirty() {
	return atomic_load(&m26_watcher_dirty);
}

void ui_status_draw() {
	graphics_fill_rect(0, 0, GRAPHICS_WIDTH, UI_STATUS_HEIGHT, GRAPHICS_COLOR_BLACK);
	graphics_draw_text("12:43", 1, 1, &font_source_sans_12, GRAPHICS_COLOR_WHITE);

	uint8_t registration = atomic_load(&m26_watcher_registration);
	uint8_t signal = atomic_load(&m26_watcher_signal);
	uint16_t voltage = atomic_load(&m26_watcher_voltage);

	graphics_point_t current_x = GRAPHICS_WIDTH;

	// from jauch datasheet: low voltage = 3.0 v, high voltage = 4.2 v
	uint8_t percentage = (uint8_t) ((((double) voltage - 3000) / ((double) (4200 - 3000))) * 100);
	uint8_t battery_level = (percentage + 20) / 20;
	if (battery_level > 5) {
		battery_level = 5;
	}

	current_x -= 2;
	current_x -= 5;
	ui_status_draw_battery(current_x, 1, battery_level);

	current_x -= 2;

	if (registration == M26_CREG_REGISTERED_NORMAL || registration == M26_CREG_REGISTERED_ROAMING) {
		int8_t rssi = -113;
		if (signal == 1) {
			rssi = -111;
		} else if (signal >= 2 && signal <= 30) {
			// -109... -53 dBm
			rssi = (2 * signal) - 113;
		} else if (signal == 31) {
			rssi = -51;
		}

		uint8_t bars = 1;
		if (rssi >= -73) {
			bars = 4;
		} else if (rssi >= -83) {
			bars = 3;
		} else if (rssi >= -93) {
			bars = 2;
		}

		current_x -= 13;
		graphics_draw_text("2G", current_x, 1, &font_source_sans_12, GRAPHICS_COLOR_WHITE);

		current_x -= 2;
		current_x -= 11;
		ui_status_draw_signal_bars(current_x, 1, bars);
	} else if (registration == M26_CREG_NOT_REGISTERED_SEARCHING) {
		current_x -= 62;
		graphics_draw_text("Searching...", current_x, 1, &font_source_sans_12, GRAPHICS_COLOR_WHITE);
	} else if (registration == M26_CREG_NOT_REGISTERED) {
		current_x -= 52;
		graphics_draw_text("No signal", current_x, 1, &font_source_sans_12, GRAPHICS_COLOR_WHITE);
	} else {
		current_x -= 32;
		graphics_draw_text("Error", current_x, 1, &font_source_sans_12, GRAPHICS_COLOR_WHITE);
	}

	atomic_store(&m26_watcher_dirty, false);
}