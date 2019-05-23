#ifndef _ICONS_ICON_H
#define _ICONS_ICON_H

#include <stdint.h>

typedef struct {
	uint8_t width;
	uint8_t height;
	uint8_t data[];
} icon_t;

extern const icon_t icon_phone;
extern const icon_t icon_sms;
extern const icon_t icon_more;

#endif