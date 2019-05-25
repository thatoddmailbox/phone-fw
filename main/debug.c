#include "debug.h"

#include <esp_log.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_event_loop.h"

#include <lwip/err.h>
#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <lwip/netdb.h>

const char * DEBUG_TAG = "debug";

#define DEBUG_HOST "192.168.0.2"
#define DEBUG_PORT 8520

#define CONFIG_ESP_WIFI_SSID "ssid"
#define CONFIG_ESP_WIFI_PASSWORD "password"

esp_err_t event_handler(void * ctx, system_event_t * event) {
    return ESP_OK;
}

void debug_init() {
    tcpip_adapter_init();

    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    wifi_config_t sta_config = {
        .sta = {
            .ssid = CONFIG_ESP_WIFI_SSID,
            .password = CONFIG_ESP_WIFI_PASSWORD,
            .bssid_set = false
        }
    };
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());

	vTaskDelay(4000 / portTICK_PERIOD_MS);
}

void debug_send_buffer(void * buffer, size_t buffer_size) {
	char addr_str[128];

	// connect to debug host

	struct sockaddr_in dest_addr;
	dest_addr.sin_addr.s_addr = inet_addr(DEBUG_HOST);
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(DEBUG_PORT);
	inet_ntoa_r(dest_addr.sin_addr, addr_str, sizeof(addr_str) - 1);

	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (sock < 0) {
		ESP_LOGE(DEBUG_TAG, "Unable to create socket: errno %d", errno);
		return;
	}
	ESP_LOGI(DEBUG_TAG, "Socket created");

	int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
	if (err != 0) {
		ESP_LOGE(DEBUG_TAG, "Socket unable to connect: errno %d", errno);
		return;
	}
	ESP_LOGI(DEBUG_TAG, "Successfully connected");

	// send the number of bytes

	// this code depends on the esp32 being little endian
	// it's really ugly
	// please never write code like this ever
	uint32_t buffer_size_out = buffer_size;
	char * buffer_size_bytes = (char *) &buffer_size_out;

	err = send(sock, buffer_size_bytes, 4, 0);
	if (err < 0) {
		ESP_LOGE(DEBUG_TAG, "Error occurred during sending: errno %d", errno);
		return;
	}
	ESP_LOGI(DEBUG_TAG, "num bytes: %d", err);

	// send the actual data
	err = send(sock, buffer, buffer_size, 0);
	if (err < 0) {
		ESP_LOGE(DEBUG_TAG, "Error occurred during sending: errno %d", errno);
		return;
	}
	ESP_LOGI(DEBUG_TAG, "buf bytes: %d", err);

	ESP_LOGI(DEBUG_TAG, "done");
	shutdown(sock, 0);
	close(sock);
}