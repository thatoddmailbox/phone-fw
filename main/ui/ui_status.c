#include "ui/ui_status.h"

static void ui_status_draw_signal_bars(uint8_t x, uint8_t y, uint8_t amount) {
	graphics_fill_rect(x, y + 6, 2, 2, GRAPHICS_COLOR_WHITE);
	graphics_fill_rect(x + 3, y + 4, 2, 4, GRAPHICS_COLOR_WHITE);
	graphics_fill_rect(x + 6, y + 2, 2, 6, GRAPHICS_COLOR_WHITE);
	graphics_fill_rect(x + 9, y, 2, 8, GRAPHICS_COLOR_WHITE);
}

void ui_status_draw() {
	graphics_fill_rect(0, 0, GRAPHICS_WIDTH, UI_STATUS_HEIGHT, GRAPHICS_COLOR_BLACK);
	graphics_draw_text("12:43", 1, 1, &font_source_sans_12, GRAPHICS_COLOR_WHITE);
	graphics_draw_text("2G", GRAPHICS_WIDTH - 13 - 2, 1, &font_source_sans_12, GRAPHICS_COLOR_WHITE);
	ui_status_draw_signal_bars(GRAPHICS_WIDTH - 13 - 2 - 2 - 11, 1, 4);
}