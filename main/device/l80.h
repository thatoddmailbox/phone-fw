#ifndef _DEVICE_L80_H
#define _DEVICE_L80_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <driver/uart.h>

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include <esp_log.h>

/*
 * pinout
 */
#define L80_RESET 2
#define L80_UART UART_NUM_1
#define L80_TX 34
#define L80_RX 5

/*
 * buffers
 */
// line buffer cannot be larger than reply buffer!!
#define L80_LINE_BUFFER_SIZE 512
#define L80_RX_BUFFER_SIZE 512

extern SemaphoreHandle_t l80_mutex;

void l80_init();
char * l80_get_next_sentence();
void l80_wait_for_next_sentence();

#endif