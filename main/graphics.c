#include "graphics.h"

uint16_t graphics_buffer[ST7735S_WIDTH * ST7735S_HEIGHT];

void graphics_init() {
	memset(graphics_buffer, 0xFF, ST7735S_WIDTH * ST7735S_HEIGHT * 2);
	graphics_flip();
}

void graphics_draw_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t thickness, graphics_color_t color) {
	for (uint8_t thickness_iter = 0; thickness_iter < thickness; thickness_iter++) {
		// top and bottom lines
		for	(uint8_t x_iter = (x + thickness_iter); x_iter <= (x + w - thickness_iter); x_iter++) {
			graphics_buffer[((y + thickness_iter) * ST7735S_WIDTH) + x_iter] = color;
			graphics_buffer[((y - thickness_iter + h) * ST7735S_WIDTH) + x_iter] = color;
		}

		// left and right lines
		for	(uint8_t y_iter = (y + thickness_iter); y_iter <= (y + h - thickness_iter); y_iter++) {
			graphics_buffer[(y_iter * ST7735S_WIDTH) + x + thickness_iter] = color;
			graphics_buffer[(y_iter * ST7735S_WIDTH) + x - thickness_iter + h] = color;
		}
	}
}

void graphics_fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, graphics_color_t color) {
	for	(uint8_t y_iter = y; y_iter < y + h; y_iter++) {
		for	(uint8_t x_iter = x; x_iter < x + w; x_iter++) {
			graphics_buffer[(y_iter * ST7735S_WIDTH) + x_iter] = color;
		}
	}
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
	uint8_t draw_y = y + top_shift;
	for (uint8_t bitmap_y = 0; bitmap_y < bitmap_height; bitmap_y++) {
		draw_x = x + left_shift;
		for (uint8_t bitmap_x = 0; bitmap_x < bitmap_width; bitmap_x++) {
			uint8_t alpha = bitmap_data[(bitmap_y * bitmap_width) + bitmap_x];
			graphics_buffer[(draw_y * GRAPHICS_WIDTH) + draw_x] = GRAPHICS_COLOR(alpha, alpha, alpha);
			draw_x++;
		}
		draw_y++;
	}
}

void graphics_draw_text(char * string, uint8_t x, uint8_t y, const font_t * font, graphics_color_t color) {
	uint8_t current_x = x;
	uint8_t current_y = y;

	while (*string) {
		char c = *string;

		const uint8_t * character_data = font->data + (font->indexes[c - '!']);
		uint8_t advance_width = character_data[4];
		// uint8_t advance_height = character_data[5];

		graphics_draw_char(c, current_x, current_y, font, color);

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