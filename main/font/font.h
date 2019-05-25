#ifndef _FONT_H
#define _FONT_H

#include <stddef.h>
#include <stdint.h>

// data format:
// topShift, leftShift, bitmapWidth, bitmapHeight, advanceWidth, advanceHeight
// data
typedef struct {
	size_t indexes['~' - ' '];
	uint8_t data[];
} font_t;

extern const font_t font_source_sans_12;
extern const font_t font_source_sans_16;

#endif
