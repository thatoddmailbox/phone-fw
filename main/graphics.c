#include "graphics.h"

uint16_t graphics_buffer[ST7735S_WIDTH * ST7735S_HEIGHT];

void graphics_init() {
	memset(graphics_buffer, 0xFF, ST7735S_WIDTH * ST7735S_HEIGHT * 2);
	graphics_flip();
}

graphics_color_t graphics_get_pixel(uint8_t x, uint8_t y) {
	return graphics_buffer[(y * ST7735S_WIDTH) + x];
}

void graphics_set_pixel(uint8_t x, uint8_t y, graphics_color_t color) {
	graphics_buffer[(y * ST7735S_WIDTH) + x] = color;
}

void graphics_draw_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t thickness, graphics_color_t color) {
	for (uint8_t thickness_iter = 0; thickness_iter < thickness; thickness_iter++) {
		// top and bottom lines
		for	(uint8_t x_iter = (x + thickness_iter); x_iter <= (x + w - thickness_iter); x_iter++) {
			graphics_set_pixel(x_iter, y + thickness_iter, color);
			graphics_set_pixel(x_iter, y - thickness_iter + h, color);
		}

		// left and right lines
		for	(uint8_t y_iter = (y + thickness_iter); y_iter <= (y + h - thickness_iter); y_iter++) {
			graphics_set_pixel(x + thickness_iter, y_iter, color);
			graphics_set_pixel(x - thickness_iter + h, y_iter, color);
		}
	}
}

void graphics_fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, graphics_color_t color) {
	for	(uint8_t y_iter = y; y_iter < y + h; y_iter++) {
		for	(uint8_t x_iter = x; x_iter < x + w; x_iter++) {
			graphics_set_pixel(x_iter, y_iter, color);
		}
	}
}

graphics_metrics_t graphics_measure_text(char * string, const font_t * font) {
	graphics_metrics_t metrics = {
		.width = 0,
		.height = 0
	};

	while (*string) {
		char c = *string;

		const uint8_t * character_data = font->data + (font->indexes[c - '!']);
		uint8_t bitmap_height = character_data[3];
		uint8_t advance_width = character_data[4];

		metrics.width += advance_width;
		if (bitmap_height > metrics.height) {
			metrics.height = bitmap_height;
		}

		string++;
	}

	return metrics;
}

void graphics_draw_char(char c, uint8_t x, uint8_t y, const font_t * font, graphics_color_t color) {
	if (c < '!' || c > '~') {
		c = '?';
	}

	const uint8_t * character_data = font->data + (font->indexes[c - '!']);
	uint8_t left_shift = character_data[0];
	uint8_t top_shift = character_data[1];
	uint8_t bitmap_width = character_data[2];
	uint8_t bitmap_height = character_data[3];
	// uint8_t advance_width = character_data[4];
	// uint8_t advance_height = character_data[5];

	const uint8_t * bitmap_data = character_data + 6;
	uint8_t draw_x = x + left_shift;
	uint8_t draw_y = y; // + top_shift;
	for (uint8_t bitmap_y = 0; bitmap_y < bitmap_height; bitmap_y++) {
		draw_x = x + left_shift;
		for (uint8_t bitmap_x = 0; bitmap_x < bitmap_width; bitmap_x++) {
			graphics_color_t background = graphics_get_pixel(draw_x, draw_y);
			uint8_t alpha = bitmap_data[(bitmap_y * bitmap_width) + bitmap_x];

			// see: https://en.wikipedia.org/wiki/Alpha_compositing
			// specifically, this is a porter-duff over operation
			// A over B where A is the text and B is the background
			// uint8_t color_r = (GRAPHICS_COLOR_GET_RED(color) + (GRAPHICS_COLOR_GET_RED(background) * (255 - alpha)));
			// uint8_t color_g = (GRAPHICS_COLOR_GET_GREEN(color) + (GRAPHICS_COLOR_GET_GREEN(background) * (255 - alpha)));
			// uint8_t color_b = (GRAPHICS_COLOR_GET_BLUE(color) + (GRAPHICS_COLOR_GET_BLUE(background) * (255 - alpha)));
			// graphics_color_t color = GRAPHICS_COLOR(color_r, color_g, color_b);

			graphics_set_pixel(draw_x, draw_y, GRAPHICS_COLOR(alpha, alpha, alpha));
			draw_x++;
		}
		draw_y++;
	}
}

void graphics_draw_text(char * string, uint8_t x, uint8_t y, const font_t * font, graphics_color_t color) {
	uint8_t current_x = x;
	uint8_t current_y = y;

	graphics_metrics_t metrics = graphics_measure_text(string, font);

	ESP_LOGI("graphics", "graphics_draw_text - %s", string);
	ESP_LOGI("graphics", "[%d, %d]", metrics.width, metrics.height);

	bool first = true;

	while (*string) {
		char c = *string;

		const uint8_t * character_data = font->data + (font->indexes[c - '!']);
		uint8_t left_shift = character_data[0];
		uint8_t bitmap_height = character_data[3];
		uint8_t advance_width = character_data[4];
		// uint8_t advance_height = character_data[5];

		uint8_t bitmap_y = current_y + (metrics.height - bitmap_height);

		if (first) {
			// first character of string, offset its left shift
			current_x -= left_shift;
			first = false;
		}

		ESP_LOGI("graphics", "(%d, %d) (cur y: %d)", current_x, bitmap_y, current_y);

		graphics_draw_char(c, current_x, bitmap_y, font, color);

		current_x += advance_width;

		string++;
	}
}

void graphics_blit_bitmap(uint8_t bitmap[ST7735S_WIDTH * ST7735S_HEIGHT]) {
	// this is BAD and relies on the bitmap having been swizzled
	memcpy(graphics_buffer, bitmap, ST7735S_WIDTH * ST7735S_HEIGHT * 2);
}

void graphics_flip() {
	st7735s_flip(graphics_buffer);
}