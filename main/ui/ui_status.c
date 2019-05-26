#include "ui/ui_status.h"

static void ui_status_draw_signal_bars(uint8_t x, uint8_t y, uint8_t amount) {
	if (amount >= 1) { graphics_fill_rect(x, y + 6, 2, 2, GRAPHICS_COLOR_WHITE); }
	if (amount >= 2) { graphics_fill_rect(x + 3, y + 4, 2, 4, GRAPHICS_COLOR_WHITE); }
	if (amount >= 3) { graphics_fill_rect(x + 6, y + 2, 2, 6, GRAPHICS_COLOR_WHITE); }
	if (amount >= 4) { graphics_fill_rect(x + 9, y, 2, 8, GRAPHICS_COLOR_WHITE); }
}

bool ui_status_dirty() {
	return atomic_load(&m26_watcher_dirty);
}

void ui_status_draw() {
	graphics_fill_rect(0, 0, GRAPHICS_WIDTH, UI_STATUS_HEIGHT, GRAPHICS_COLOR_BLACK);
	graphics_draw_text("12:43", 1, 1, &font_source_sans_12, GRAPHICS_COLOR_WHITE);

	uint8_t registration = atomic_load(&m26_watcher_registration);
	uint8_t signal = atomic_load(&m26_watcher_signal);

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

		graphics_draw_text("2G", GRAPHICS_WIDTH - 13 - 2, 1, &font_source_sans_12, GRAPHICS_COLOR_WHITE);
		ui_status_draw_signal_bars(GRAPHICS_WIDTH - 13 - 2 - 2 - 11, 1, bars);
	} else if (registration == M26_CREG_NOT_REGISTERED_SEARCHING) {
		graphics_draw_text("Searching...", GRAPHICS_WIDTH - 62 - 2, 1, &font_source_sans_12, GRAPHICS_COLOR_WHITE);
	} else if (registration == M26_CREG_NOT_REGISTERED) {
		graphics_draw_text("No signal", GRAPHICS_WIDTH - 52 - 2, 1, &font_source_sans_12, GRAPHICS_COLOR_WHITE);
	} else {
		graphics_draw_text("Error", GRAPHICS_WIDTH - 32 - 2, 1, &font_source_sans_12, GRAPHICS_COLOR_WHITE);
	}

	atomic_store(&m26_watcher_dirty, false);
}