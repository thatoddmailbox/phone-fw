#include "graphics.h"

graphics_color_t graphics_buffer[ST7735S_WIDTH * ST7735S_HEIGHT];

void graphics_init() {
	memset(graphics_buffer, 0xFF, ST7735S_WIDTH * ST7735S_HEIGHT * sizeof(graphics_color_t));
	graphics_flip();
}

void graphics_clear(graphics_color_t color) {
	for (int i = 0; i < (sizeof(graphics_buffer)/sizeof(graphics_color_t)); i++) {
		graphics_buffer[i] = color;
	}
}

// colorspace conversion functions are modified versions of the code from https://www.cs.rit.edu/~ncs/color/t_convert.html

static void graphics_rgb_to_hsv( float r, float g, float b, float *h, float *s, float *v ) {
	float color_min, color_max, delta;

	color_min = fminf(r, fminf(g, b));
	color_max = fmaxf(r, fmaxf(g, b));
	*v = color_max;

	delta = color_max - color_min;

	if( color_max != 0 )
		*s = delta / color_max;
	else {
		// r = g = b = 0		// s = 0, v is undefined
		*s = 0;
		*h = -1;
		return;
	}

	if (delta != 0) {
		if( r == color_max )
			*h = ( g - b ) / delta;		// between yellow & magenta
		else if( g == color_max )
			*h = 2 + ( b - r ) / delta;	// between cyan & yellow
		else
			*h = 4 + ( r - g ) / delta;	// between magenta & cyan
	} else {
		*h = 0; // white?
	}

	*h *= 60;				// degrees
	if( *h < 0 )
		*h += 360;

}

static void graphics_hsv_to_rgb(float *r, float *g, float *b, float h, float s, float v) {
	int i;
	float f, p, q, t;

	if( s == 0 ) {
		// achromatic (grey)
		*r = *g = *b = v;
		return;
	}

	h /= 60;			// sector 0 to 5
	i = floor( h );
	f = h - i;			// factorial part of h
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );

	switch( i ) {
		case 0:
			*r = v;
			*g = t;
			*b = p;
			break;
		case 1:
			*r = q;
			*g = v;
			*b = p;
			break;
		case 2:
			*r = p;
			*g = v;
			*b = t;
			break;
		case 3:
			*r = p;
			*g = q;
			*b = v;
			break;
		case 4:
			*r = t;
			*g = p;
			*b = v;
			break;
		default:		// case 5:
			*r = v;
			*g = p;
			*b = q;
			break;
	}
}

graphics_color_t graphics_interpolate_color(graphics_color_t start, graphics_color_t end, uint8_t amount) {
	if (amount == 0) {
		return start;
	}
	if (amount == 255) {
		return end;
	}

	float start_r, start_g, start_b, end_r, end_g, end_b;
	start_r = ((float) GRAPHICS_COLOR_GET_RED(start)) / 255;
	start_g = ((float) GRAPHICS_COLOR_GET_GREEN(start)) / 255;
	start_b = ((float) GRAPHICS_COLOR_GET_BLUE(start)) / 255;
	end_r = ((float) GRAPHICS_COLOR_GET_RED(end)) / 255;
	end_g = ((float) GRAPHICS_COLOR_GET_GREEN(end)) / 255;
	end_b = ((float) GRAPHICS_COLOR_GET_BLUE(end)) / 255;

	float start_h, start_s, start_v, end_h, end_s, end_v;

	graphics_rgb_to_hsv(start_r, start_g, start_b, &start_h, &start_s, &start_v);
	graphics_rgb_to_hsv(end_r, end_g, end_b, &end_h, &end_s, &end_v);

	float amount_norm = ((float) amount) / 255;

	float interp_h = ((end_h - start_h) * amount_norm) + start_h;
	float interp_s = ((end_s - start_s) * amount_norm) + start_s;
	float interp_v = ((end_v - start_v) * amount_norm) + start_v;

	float interp_r, interp_g, interp_b;

	graphics_hsv_to_rgb(&interp_r, &interp_g, &interp_b, interp_h, interp_s, interp_v);

	return GRAPHICS_COLOR((uint8_t) (interp_r * 255), (uint8_t) (interp_g * 255), (uint8_t) (interp_b * 255));
}

graphics_color_t graphics_get_pixel(uint8_t x, uint8_t y) {
	return graphics_buffer[(y * ST7735S_WIDTH) + x];
}

void graphics_set_pixel(uint8_t x, uint8_t y, graphics_color_t color) {
	if (x >= ST7735S_WIDTH || y >= ST7735S_HEIGHT) {
		return;
	}
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
			uint8_t alpha =  bitmap_data[(bitmap_y * bitmap_width) + bitmap_x];

			graphics_color_t pixel_color;
			if (background == GRAPHICS_COLOR_WHITE && color == GRAPHICS_COLOR_BLACK) {
				// fast path: skip interpolation calculations
				pixel_color = GRAPHICS_COLOR(alpha, alpha, alpha);
			} else {
				pixel_color = graphics_interpolate_color(background, color, 255 - alpha);
			}

			graphics_set_pixel(draw_x, draw_y, pixel_color);
			draw_x++;
		}
		draw_y++;
	}
}

void graphics_draw_text(char * string, uint8_t x, uint8_t y, const font_t * font, graphics_color_t color) {
	uint8_t current_x = x;
	uint8_t current_y = y;

	graphics_metrics_t metrics = graphics_measure_text(string, font);

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

		graphics_draw_char(c, current_x, bitmap_y, font, color);

		current_x += advance_width;

		string++;
	}
}

void graphics_draw_icon(uint8_t x, uint8_t y, const icon_t * icon) {
	for (size_t y_iter = 0; y_iter < icon->height; y_iter++) {
		for (size_t x_iter = 0; x_iter < icon->width; x_iter++) {
			uint16_t data_index = 2 * ((y_iter * icon->width) + x_iter);
			graphics_set_pixel(x + x_iter, y + y_iter, (((uint16_t) icon->data[data_index + 1]) << 8) | (((uint16_t) icon->data[data_index]) & 0xFF));
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