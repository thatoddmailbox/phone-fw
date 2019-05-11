#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "driver/uart.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "m26.h"
#include "mcp23008.h"
#include "st7735s.h"

#include "input.h"
#include "ui.h"

#define EXAMPLE_ESP_WIFI_SSID      "yay wifi"
#define EXAMPLE_ESP_WIFI_PASS      "password12"
#define EXAMPLE_MAX_STA_CONN       10

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

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

static const char *TAG = "softAP";

static esp_err_t event_handler(void *ctx, system_event_t *event) {
    switch(event->event_id) {
    case SYSTEM_EVENT_AP_STACONNECTED:
        ESP_LOGI(TAG, "station:"MACSTR" join, AID=%d",
                 MAC2STR(event->event_info.sta_connected.mac),
                 event->event_info.sta_connected.aid);
        break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
        ESP_LOGI(TAG, "station:"MACSTR"leave, AID=%d",
                 MAC2STR(event->event_info.sta_disconnected.mac),
                 event->event_info.sta_disconnected.aid);
        break;
    default:
        break;
    }
	return ESP_OK;
}

void wifi_init_softap() {
    s_wifi_event_group = xEventGroupCreate();

    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID),
            .password = EXAMPLE_ESP_WIFI_PASS,
            .max_connection = EXAMPLE_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    if (strlen(EXAMPLE_ESP_WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_softap finished.SSID:%s password:%s", EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
}

void app_main() {
    printf("Hello world!\n");

    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

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
	ui_init();

	m26_init();

	while (1) {
		input_step();
		ui_step();

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

/*

	ESP_LOGI("hi", "boonking pwrkey\n");

	// low for 1s
	// gpio_set_level(GSM_PWRKEY, 0);
	// vTaskDelay(1100 / portTICK_PERIOD_MS);
	// gpio_set_level(GSM_PWRKEY, 1);

	ESP_LOGI("hi", "pwrkey boonked, trying to talk\n");
	vTaskDelay(100 / portTICK_PERIOD_MS);

	// serial comms

	// while (1) {
	char* test_str = "AT\n";
	char* aux_uart_str = "AT+QEAUART=1\n";

	ESP_LOGI("hi", "first AT\n");
	uart_write_bytes(GSM_UART, (const char*)test_str, strlen(test_str));
	ESP_ERROR_CHECK(uart_wait_tx_done(GSM_UART, 100));
	vTaskDelay(4000 / portTICK_PERIOD_MS);
	
	ESP_LOGI("hi", "second AT\n");
	uart_write_bytes(GSM_UART, (const char*)test_str, strlen(test_str));
	ESP_ERROR_CHECK(uart_wait_tx_done(GSM_UART, 100));
	vTaskDelay(4000 / portTICK_PERIOD_MS);
	// }
	
	ESP_LOGI("hi", "extra uart AT\n");
	uart_write_bytes(GSM_UART, (const char*)aux_uart_str, strlen(aux_uart_str));
	ESP_ERROR_CHECK(uart_wait_tx_done(GSM_UART, 100));

*/

/*
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
		
		vTaskDelay(1 / portTICK_PERIOD_MS);
	}
*/

	ESP_LOGI("hi", "done, starting softap i guess\n");

	wifi_init_softap();
}
