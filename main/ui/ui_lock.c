#include "ui_lock.h"

void ui_lock_draw() {
    graphics_blit_bitmap(image_lock_bg);
    graphics_draw_text("12:34", 10, 20, &font_source_sans_16, GRAPHICS_COLOR_BLACK);

	graphics_fill_rect(0, 20 + (16 * 1), GRAPHICS_WIDTH, 14, GRAPHICS_COLOR_WHITE);
    graphics_draw_text("No Wi-Fi", 10, 20 + (16 * 1) + 2, &font_source_sans_12, GRAPHICS_COLOR_BLACK);

	graphics_fill_rect(0, 20 + (16 * 2), GRAPHICS_WIDTH, 14, GRAPHICS_COLOR_WHITE);
    graphics_draw_text("T-Mobile USA", 10, 20 + (16 * 2) + 2, &font_source_sans_12, GRAPHICS_COLOR_BLACK);

    graphics_point_t unlock_y = 105;

	graphics_fill_rect(0, unlock_y, GRAPHICS_WIDTH, 23, GRAPHICS_COLOR_WHITE);
    graphics_metrics_t first_line = graphics_measure_text("Press SELECT", &font_source_sans_12);
    graphics_draw_text("Press SELECT", (GRAPHICS_WIDTH - first_line.width) / 2, unlock_y + 2, &font_source_sans_12, GRAPHICS_COLOR_BLACK);
    graphics_metrics_t second_line = graphics_measure_text("to unlock", &font_source_sans_12);
    graphics_draw_text("to unlock", (GRAPHICS_WIDTH - second_line.width) / 2, unlock_y + 2 + first_line.height + 2, &font_source_sans_12, GRAPHICS_COLOR_BLACK);

    graphics_flip();
}