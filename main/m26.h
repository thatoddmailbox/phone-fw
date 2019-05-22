#ifndef _M26_H
#define _M26_H

#include <stdint.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_log.h"

#include "driver/uart.h"

#define M26_PWRKEY 4
#define M26_UART UART_NUM_2
#define M26_TX 35
#define M26_RX 18

// line buffer cannot be larger than reply buffer!!
#define M26_LINE_BUFFER_SIZE 256
#define M26_REPLY_BUFFER_SIZE 256

#define M26_DEFAULT_TIMEOUT 400

void m26_init();

size_t m26_get_line(uint16_t timeout);

void m26_send_command(const char * command);
void m26_send_single_reply(const char * command);

void m26_get_ccid(char * ccid); // must be at least 21 bytes long
void m26_get_imei(char * imei); // must be at least 16 bytes long
char * m26_get_operator();

void m26_gprs_activate(char * apn);
void m26_http_get(char * url);
void m26_gprs_deactivate();

#endif