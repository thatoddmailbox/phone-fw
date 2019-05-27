#include "app/launch/main.h"

const char * APP_LAUNCH_TAG = "launch";

ui_screen_t * app_launch_main;

static void app_launch_button_click(ui_list_item_t * list_item, ui_item_t * list, ui_screen_t * screen) {
	ESP_LOGI(APP_LAUNCH_TAG, "button pressed: %s", list_item->label);
}

static void app_launch_init() {
	app_launch_main = ui_screen_new("Launcher");

	ui_item_t * list = ui_list_new(0, UI_STATUS_HEIGHT + UI_SCREEN_TITLE_HEIGHT, GRAPHICS_WIDTH, GRAPHICS_HEIGHT - (UI_STATUS_HEIGHT + UI_SCREEN_TITLE_HEIGHT));

	ui_list_metadata_t * list_data = (ui_list_metadata_t *) list->metadata;

	list_add(list_data->items, ui_list_item_new("Phone", &icon_phone, &app_launch_button_click));
	list_add(list_data->items, ui_list_item_new("SMS", &icon_sms, &app_launch_button_click));
	list_add(list_data->items, ui_list_item_new("Other", &icon_more, &app_launch_button_click));

	app_launch_main->list = (void *) list;

	// ui_item_t * phone = ui_button_new("Phone", &icon_phone, 10, 10 + 20 + 5, 32, 32, app_launch_button_click);
	// list_add(app_launch_main->fg, phone);

	// ui_item_t * sms = ui_button_new("SMS", &icon_sms, 10, 10 + 20 + 5 + ((32 + 10) * 1), 32, 32, app_launch_button_click);
	// list_add(app_launch_main->fg, sms);

	// ui_item_t * other = ui_button_new("Other", &icon_more, 10, 10 + 20 + 5 + ((32 + 10) * 2), 32, 32, app_launch_button_click);
	// list_add(app_launch_main->fg, other);
}

static void app_launch_start() {
	ui_set_screen(app_launch_main);
}

app_t app_launch = {
	.name = "Launcher",

	.init = &app_launch_init,
	.start = &app_launch_start
};