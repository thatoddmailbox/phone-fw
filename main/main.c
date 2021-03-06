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
#include "expio.h"
#include "graphics.h"
#include "input.h"

#include "app/main.h"
#include "app/launch/main.h"

#include "device/l80.h"
#include "device/m26.h"
#include "device/m26_transport.h"
#include "device/mcp23008.h"
#include "device/pcal6416.h"
#include "device/st7735s.h"

#include "hwconfig.h"

#include "ui/ui.h"
#include "ui/ui_lock.h"

#include "watcher/l80_watcher.h"
#include "watcher/m26_watcher.h"

void app_main() {
	esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
	ESP_ERROR_CHECK(ret);

	// TODO: not hardcode this
	setenv("TZ", "EST5EDT,M3.2.0/2,M11.1.0", 1);
	tzset();

	// start up ui stuff first
#if HW_VERSION == EVAL_HW
	mcp23008_init();
#else
	pcal6416_init();
	expio_init();
#endif
	st7735s_init();
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

	// start watcher tasks
	m26_watcher_start();
	l80_watcher_start();

	app_init();

	app_launch.start();

	while (1) {
		input_step();
		ui_step();

		vTaskDelay(1);
	}
}
