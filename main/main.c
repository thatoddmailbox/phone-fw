#include <stdio.h>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_system.h>
#include <esp_spi_flash.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <nvs_flash.h>

#include <driver/uart.h>

#include "device/m26.h"
#include "device/m26_transport.h"
#include "device/mcp23008.h"
#include "device/st7735s.h"

#include "debug.h"
#include "graphics.h"
#include "input.h"

#include "app/main.h"
#include "app/launch/main.h"

#include "ui/ui_button.h"
#include "ui/ui.h"

/*
 * helpful pins:
 * 
 * M26:
 * GSM_PWRKEY: GPIO4
 * UART2_RX/GSM_TX: GPIO35
 * UART2_TX/GSM_RX: GPIO18
 * 
 * L80-R:
 * GPS_RESET: GPIO2
 * UART1_RX/GPS_TX: GPIO34
 * UART1_TX/GPS_RX: GPIO5
 */

#define GPS_RESET 2
#define GPS_UART UART_NUM_1
#define GPS_TX 34
#define GPS_RX 5

void app_main() {
	esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
	ESP_ERROR_CHECK( ret );

	ESP_LOGI("hi", "setting stuff up");

	// start up ui stuff first
	st7735s_init();
	mcp23008_init();
	input_init();
	graphics_init();
	ui_init();

	// debug_init();

	graphics_draw_text("Loading...", 10, 10, &font_source_sans_16, GRAPHICS_COLOR_BLACK);
	graphics_flip();

	// m26_init();

	// m26_gprs_activate("hologram");
	// m26_dns_set("8.8.8.8", "8.8.8.4");

	// // m26_http_get("http://aserv-cloud.cloudapp.net/phone/ping.php");
	// // m26_tcp_open("studerfamily.us", 9999);
	// // m26_tcp_write("Yay it works!\n", 14);
	// // m26_tcp_close();

	// m26_gprs_deactivate();

	app_init();

	app_launch.start();

	bool yay = true;

	while (1) {
		input_step();
		ui_step();

		if (yay) {
			// debug_send_buffer(graphics_buffer, sizeof(graphics_buffer));
			yay = false;
		}

		vTaskDelay(1);
	}

	// talk to m26

	char imei[16];
	m26_get_imei(imei);
	ESP_LOGI("hi", "imei = %s", imei);

	char ccid[21];
	ccid[0] = '\0';
	m26_get_ccid(ccid);
	ESP_LOGI("hi", "ccid = %s", ccid);

	char * op = m26_get_operator();
	if (op) {
		ESP_LOGI("hi", "operator = %s", op);
		free(op);
	} else {
		ESP_LOGI("hi", "operator = null");
	}

	// set up gps
	uart_config_t gps_uart_config = {
		.baud_rate = 9600,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.rx_flow_ctrl_thresh = 122,
	};

	// Configure UART parameters - l80
	ESP_ERROR_CHECK(uart_param_config(GPS_UART, &gps_uart_config));
	ESP_ERROR_CHECK(uart_set_pin(GPS_UART, GPS_RX, GPS_TX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

	const int uart_buffer_size = (1024 * 2);
	QueueHandle_t gps_uart_queue;
	// Install UART driver using an event queue here
	ESP_ERROR_CHECK(uart_driver_install(GPS_UART, uart_buffer_size, uart_buffer_size, 10, &gps_uart_queue, 0));

	while (1) {
		uint8_t data[128];
		size_t length = 0;

		// gps read
		length = 0;
		ESP_ERROR_CHECK(uart_get_buffered_data_len(GPS_UART, &length));
		length = uart_read_bytes(GPS_UART, data, length, 100);
		if (length > 0) {
			data[length] = '\0';
			ESP_LOGI("gps", "got something %d!\n", length);
			printf("gpsyay: %s\n", (char *) data);
		}
		
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}
