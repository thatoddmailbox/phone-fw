#ifndef _EXPIO_H
#define _EXPIO_H

#include <stdint.h>

#include "device/pcal6416.h"

#define EXPIO_BATT_STDBY 0
#define EXPIO_INPUT_LOCK 1
#define EXPIO_INPUT_VOL_UP 2
#define EXPIO_INPUT_VOL_DOWN 3
#define EXPIO_LCD_RESET 4
#define EXPIO_LCD_BL 5
#define EXPIO_GPS_RESET 6
#define EXPIO_GSM_PWRKEY 7

#define EXPIO_OUTPUT 0
#define EXPIO_INPUT 1

void expio_init();
void expio_set_direction(uint8_t pin, uint8_t direction);
uint8_t expio_get_level(uint8_t pin);
void expio_set_level(uint8_t pin, uint8_t level);

#endif