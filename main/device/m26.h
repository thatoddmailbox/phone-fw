#ifndef _DEVICE_M26_H
#define _DEVICE_M26_H

#include <stdint.h>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>

#include <esp_log.h>

#include <driver/uart.h>

#include "expio.h"
#include "hwconfig.h"

#include "device/pcal6416.h"

/*
 * pinout
 */
#define M26_PWRKEY 4
#define M26_UART UART_NUM_2
#define M26_TX 35
#define M26_RX 18

/*
 * buffers
 */
#define M26_TX_BUFFER_SIZE 256
// line buffer cannot be larger than reply buffer!!
#define M26_LINE_BUFFER_SIZE 256
#define M26_RX_BUFFER_SIZE 256

/*
 * other
 */
#define M26_DEFAULT_TIMEOUT 400

/*
 * constants
 */
#define M26_CREG_NOT_REGISTERED 0
#define M26_CREG_REGISTERED_NORMAL 1
#define M26_CREG_NOT_REGISTERED_SEARCHING 2
#define M26_CREG_REGISTRATION_DENIED 3
#define M26_CREG_NO_COVERAGE 4
#define M26_CREG_REGISTERED_ROAMING 5

extern SemaphoreHandle_t m26_mutex;

void m26_init();

size_t m26_get_line(uint16_t timeout);

void m26_write_data(const char * data, size_t length);
void m26_send_command(const char * command);
void m26_send_single_reply(const char * command);

void m26_get_ccid(char * ccid); // must be at least 21 bytes long
uint8_t m26_get_creg();
uint8_t m26_get_csq();
void m26_get_imei(char * imei); // must be at least 16 bytes long
char * m26_get_operator();

void m26_gprs_activate(const char * apn);
void m26_gprs_deactivate();

void m26_dns_set(const char * primary, const char * secondary);

void m26_tcp_open(const char * host, uint16_t port);
void m26_tcp_write(const char * data, size_t length);
void m26_tcp_close();

void m26_http_get(const char * url);

#endif