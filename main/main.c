#include <stdio.h>
#include <string.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_system.h>
#include <esp_spi_flash.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <nvs_flash.h>

#include "debug.h"
#include "graphics.h"
#include "input.h"

#include "app/main.h"
#include "app/launch/main.h"

#include "device/l80.h"
#include "device/m26.h"
#include "device/m26_transport.h"
#include "device/mcp23008.h"
#include "device/st7735s.h"

#include "ui/ui.h"

#include "watcher/l80_watcher.h"
#include "watcher/m26_watcher.h"

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

	// show something on the screen to entertain user
	graphics_draw_text("Loading...", 10, 10, &font_source_sans_16, GRAPHICS_COLOR_BLACK);
	graphics_flip();

	// start devices
	m26_init();
	l80_init();

	// start watcher tasks
	m26_watcher_start();
	l80_watcher_start();

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
}
