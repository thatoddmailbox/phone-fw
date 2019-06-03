#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#include <stdint.h>

#include <math.h>

#include "device/st7735s.h"

#include "font/font.h"

#include "icon/icon.h"

typedef uint16_t graphics_color_t;
typedef int16_t graphics_point_t;

typedef struct {
	uint8_t width;
	uint8_t height;
} graphics_metrics_t;

extern graphics_color_t graphics_buffer[ST7735S_WIDTH * ST7735S_HEIGHT];

#define GRAPHICS_SWIZZLE_16(N) (((N & 0xFF) << 8) | (N >> 8))

// color conversion from https://stackoverflow.com/questions/11471122/rgb888-to-rgb565-bit-shifting
#define GRAPHICS_COLOR(R, G, B) (graphics_color_t) GRAPHICS_SWIZZLE_16( (((R & 0b11111000) << 8) | ((G & 0b11111100) << 3) | (B >> 3)) )
#define GRAPHICS_COLOR_GET_RED(COLOR) (uint8_t) (((COLOR & 0b1111100000000000) >> 8) | ((COLOR & 0b1111100000000000) >> 13))
#define GRAPHICS_COLOR_GET_GREEN(COLOR) (uint8_t) (((COLOR & 0b0000011111100000) >> 3) | ((COLOR & 0b0000011111100000) >> 9))
#define GRAPHICS_COLOR_GET_BLUE(COLOR) (uint8_t) (((COLOR & 0b0000000000011111) << 3) | ((COLOR & 0b0000000000011111) >> 2))

#define GRAPHICS_COLOR_BLACK GRAPHICS_COLOR(0, 0, 0)
#define GRAPHICS_COLOR_DARK_GRAY GRAPHICS_COLOR(64, 64, 64)
#define GRAPHICS_COLOR_GRAY GRAPHICS_COLOR(128, 128, 128)
#define GRAPHICS_COLOR_LIGHT_GRAY GRAPHICS_COLOR(192, 192, 192)
#define GRAPHICS_COLOR_RED GRAPHICS_COLOR(255, 0, 0)
#define GRAPHICS_COLOR_GREEN GRAPHICS_COLOR(0, 255, 0)
#define GRAPHICS_COLOR_BLUE GRAPHICS_COLOR(0, 0, 255)
#define GRAPHICS_COLOR_WHITE GRAPHICS_COLOR(255, 255, 255)

#define GRAPHICS_WIDTH ST7735S_WIDTH
#define GRAPHICS_HEIGHT ST7735S_HEIGHT

void graphics_init();

void graphics_clear(graphics_color_t color);

graphics_color_t graphics_interpolate_color(graphics_color_t start, graphics_color_t end, uint8_t amount);

graphics_color_t graphics_get_pixel(graphics_point_t x, graphics_point_t y);
void graphics_set_pixel(graphics_point_t x, graphics_point_t y, graphics_color_t color);

void graphics_draw_rect(graphics_point_t x, graphics_point_t y, graphics_point_t w, graphics_point_t h, uint8_t thickness, graphics_color_t color);
void graphics_fill_rect(graphics_point_t x, graphics_point_t y, graphics_point_t w, graphics_point_t h, graphics_color_t color);

graphics_metrics_t graphics_measure_text(char * string, const font_t * font);

void graphics_draw_char(char c, graphics_point_t x, graphics_point_t y, const font_t * font, graphics_color_t color);
void graphics_draw_text(const char * string, graphics_point_t x, graphics_point_t y, const font_t * font, graphics_color_t color);
void graphics_draw_icon(graphics_point_t x, graphics_point_t y, const icon_t * icon);

void graphics_blit_bitmap(const uint8_t bitmap[ST7735S_WIDTH * ST7735S_HEIGHT * 2]);

void graphics_flip();

#endif