#ifndef _GRAPHICS_H
#define _GRAPHICS_H

#include <stdint.h>

#include "st7735s.h"

typedef uint16_t graphics_color_t;

#define GRAPHICS_SWIZZLE_16(N) (((N & 0xFF) << 8) | (N >> 8))

// color conversion from https://stackoverflow.com/questions/11471122/rgb888-to-rgb565-bit-shifting
#define GRAPHICS_COLOR(R, G, B) (graphics_color_t) GRAPHICS_SWIZZLE_16( (((R & 0b11111000) << 8) | ((G & 0b11111100) << 3) | (B >> 3)) )

#define GRAPHICS_COLOR_BLACK GRAPHICS_COLOR(0, 0, 0)
#define GRAPHICS_COLOR_RED GRAPHICS_COLOR(255, 0, 0)
#define GRAPHICS_COLOR_GREEN GRAPHICS_COLOR(0, 255, 0)
#define GRAPHICS_COLOR_BLUE GRAPHICS_COLOR(0, 0, 255)
#define GRAPHICS_COLOR_WHITE GRAPHICS_COLOR(255, 255, 255)

#define GRAPHICS_WIDTH ST7735S_WIDTH
#define GRAPHICS_HEIGHT ST7735S_HEIGHT

void graphics_init();

void graphics_draw_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t thickness, graphics_color_t color);
void graphics_fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, graphics_color_t color);

void graphics_blit_bitmap(uint8_t bitmap[ST7735S_WIDTH * ST7735S_HEIGHT]);

void graphics_flip();

#endif