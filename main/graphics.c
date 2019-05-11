#include "graphics.h"

uint16_t graphics_buffer[ST7735S_WIDTH * ST7735S_HEIGHT];

void graphics_init() {
	memset(graphics_buffer, 0xFF, ST7735S_WIDTH * ST7735S_HEIGHT * 2);
	graphics_flip();
}

void graphics_draw_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, graphics_color_t color) {
	// top and bottom lines
	for	(uint8_t x_iter = x; x_iter < x + w; x_iter++) {
		graphics_buffer[(y * ST7735S_WIDTH) + x_iter] = color;
		graphics_buffer[((y + h) * ST7735S_WIDTH) + x_iter] = color;
	}

	// left and right lines
	for	(uint8_t y_iter = y; y_iter < y + h; y_iter++) {
		graphics_buffer[(y_iter * ST7735S_WIDTH) + x] = color;
		graphics_buffer[(y_iter * ST7735S_WIDTH) + x + h] = color;
	}
}

void graphics_fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, graphics_color_t color) {
	for	(uint8_t y_iter = y; y_iter < y + h; y_iter++) {
		for	(uint8_t x_iter = x; x_iter < x + w; x_iter++) {
			graphics_buffer[(y_iter * ST7735S_WIDTH) + x_iter] = color;
		}
	}
}

void graphics_blit_bitmap(uint8_t bitmap[ST7735S_WIDTH * ST7735S_HEIGHT]) {
	// this is BAD and relies on the bitmap having been swizzled
	memcpy(graphics_buffer, bitmap, ST7735S_WIDTH * ST7735S_HEIGHT * 2);
}

void graphics_flip() {
	st7735s_flip(graphics_buffer);
}