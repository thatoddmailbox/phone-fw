#include "app/launch/main.h"

const char * APP_LAUNCH_TAG = "launch";

ui_screen_t * app_launch_main;

void app_launch_button_click(ui_item_t * item, ui_screen_t * screen) {
	ESP_LOGI(APP_LAUNCH_TAG, "button pressed: %s", ((ui_button_metadata_t *) item->metadata)->text);
}

void app_launch_init() {
	app_launch_main = ui_screen_new("Launcher");

	ui_item_t * phone = ui_button_new("Phone", 10, 10, 32, 32, app_launch_button_click);
	list_add(app_launch_main->fg, phone);

	ui_item_t * sms = ui_button_new("SMS", 10, 10 + ((32 + 10) * 1), 32, 32, app_launch_button_click);
	list_add(app_launch_main->fg, sms);

	ui_item_t * other = ui_button_new("Other", 10, 10 + ((32 + 10) * 2), 32, 32, app_launch_button_click);
	list_add(app_launch_main->fg, other);
}

void app_launch_start() {
	ui_set_screen(app_launch_main);
}