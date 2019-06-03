#ifndef _DEVICE_ST7735S_H
#define _DEVICE_ST7735S_H

#include <stdint.h>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_log.h>

#include <driver/spi_master.h>

#include "expio.h"
#include "hwconfig.h"

#define ST7735S_CS 19
#define ST7735S_SDA 22
#define ST7735S_SCL 21
#define ST7735S_DC 23
#define ST7735S_BL 33

#define ST7735S_WIDTH 128
#define ST7735S_HEIGHT 128

void st7735s_init();

void st7735s_write_command(uint8_t command);
void st7735s_write_data(uint8_t data);
void st7735s_write_data_16(uint16_t data);

void st7735s_set_window(uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1);
void st7735s_flip();

#endif