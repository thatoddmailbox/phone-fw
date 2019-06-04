#include "app/launch/main.h"

#include "app/sms/main.h"

const char * APP_LAUNCH_TAG = "launch";

ui_screen_t * app_launch_main;
ui_screen_t * app_launch_other;

static void app_launch_main_activate(ui_list_item_t * list_item, ui_item_t * list, ui_screen_t * screen) {
	if (strcmp(list_item->label, "Phone") == 0) {

	} else if (strcmp(list_item->label, "SMS") == 0) {
		app_sms.start();
	} else if (strcmp(list_item->label, "Other") == 0) {
		ui_set_screen(app_launch_other);
	}
}

static void app_launch_other_activate(ui_list_item_t * list_item, ui_item_t * list, ui_screen_t * screen) {
	if (strcmp(list_item->label, "Phone") == 0) {

	} else if (strcmp(list_item->label, "SMS") == 0) {

	} else if (strcmp(list_item->label, "Other") == 0) {

	}
}

static void app_launch_other_back(ui_screen_t * screen) {
	ui_set_screen(app_launch_main);
}

static void app_launch_init() {
	/*
	 * main
	 */
	app_launch_main = ui_screen_new("Launcher");

	ui_item_t * main_list = ui_list_new(0, UI_STATUS_HEIGHT + UI_SCREEN_TITLE_HEIGHT, GRAPHICS_WIDTH, GRAPHICS_HEIGHT - (UI_STATUS_HEIGHT + UI_SCREEN_TITLE_HEIGHT));

	ui_list_metadata_t * main_list_data = (ui_list_metadata_t *) main_list->metadata;

	list_add(main_list_data->items, ui_list_item_new("Phone", &icon_phone, &app_launch_main_activate));
	list_add(main_list_data->items, ui_list_item_new("SMS", &icon_sms, &app_launch_main_activate));
	list_add(main_list_data->items, ui_list_item_new("Other", &icon_more, &app_launch_main_activate));

	app_launch_main->list = (void *) main_list;

	/*
	 * other
	 */
	app_launch_other = ui_screen_new("Other");

	ui_item_t * other_list = ui_list_new(0, UI_STATUS_HEIGHT + UI_SCREEN_TITLE_HEIGHT, GRAPHICS_WIDTH, GRAPHICS_HEIGHT - (UI_STATUS_HEIGHT + UI_SCREEN_TITLE_HEIGHT));

	ui_list_metadata_t * other_list_data = (ui_list_metadata_t *) other_list->metadata;

	list_add(other_list_data->items, ui_list_item_new("Test", &icon_more, &app_launch_other_activate));

	app_launch_other->list = (void *) other_list;
	app_launch_other->go_back = &app_launch_other_back;
}

static void app_launch_start() {
	ui_set_screen(app_launch_main);
}

app_t app_launch = {
	.name = "Launcher",

	.init = &app_launch_init,
	.start = &app_launch_start
};