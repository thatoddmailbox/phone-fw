#include "app/sms/main.h"

#include "app/launch/main.h"

const char * APP_SMS_TAG = "sms";

ui_screen_t * app_sms_main;
ui_screen_t * app_sms_reply;

bool app_sms_saved_sms = false;
uint16_t app_sms_count = 0;
m26_sms_t * app_sms_list[3];

static void app_sms_main_activate(ui_list_item_t * list_item, ui_item_t * list, ui_screen_t * screen) {
	ui_set_screen(app_sms_reply);
}

static void app_sms_main_back(ui_screen_t * screen) {
	app_launch.start();
}

static void app_sms_reply_activate(ui_list_item_t * list_item, ui_item_t * list, ui_screen_t * screen) {
	ESP_LOGI("sms", "label %s", list_item->label);
}

static void app_sms_reply_back(ui_screen_t * screen) {
	ui_set_screen(app_sms_main);
}

static void app_sms_draw(ui_screen_t * screen) {
	xSemaphoreTake(m26_mutex, portMAX_DELAY);

	if (!app_sms_saved_sms) {
		app_sms_count = m26_get_sms_count();

		app_sms_list[0] = NULL;
		app_sms_list[1] = NULL;
		app_sms_list[2] = NULL;

		uint16_t current_sms = app_sms_count;
		for (size_t i = 0; i < 3; i++) {
			m26_sms_t * sms = m26_get_sms(current_sms);

			app_sms_list[i] = sms;

			if (current_sms == 0) {
				break;
			}
			current_sms--;
		}

		app_sms_saved_sms = true;
	}

	for (size_t i = 0; i < 3; i++) {
		m26_sms_t * sms = app_sms_list[i];

		if (!sms) {
			continue;
		}

		ESP_LOGI(APP_SMS_TAG, "from %s", sms->from);
		ESP_LOGI(APP_SMS_TAG, "timestamp %s", sms->timestamp);
		ESP_LOGI(APP_SMS_TAG, "message %s", sms->message);

		graphics_point_t base_y = UI_STATUS_HEIGHT + UI_SCREEN_TITLE_HEIGHT + 3;

		graphics_draw_text(sms->from, 5, base_y + (i * 50), &font_source_sans_16, GRAPHICS_COLOR_BLACK);
		graphics_draw_text(sms->timestamp, 5, base_y + (i * 50) + 12, &font_source_sans_12, GRAPHICS_COLOR_BLACK);
		graphics_draw_text(sms->message, 5, base_y + (i * 50) + 12 + 12, &font_source_sans_12, GRAPHICS_COLOR_BLACK);

		for (size_t i = 0; i < strlen(sms->message); i++) {
			ESP_LOGI(APP_SMS_TAG, "char %d - '%c' (%d)", i, sms->message[i], sms->message[i]);
		}
	}

	xSemaphoreGive(m26_mutex);
}

static void app_sms_main_deinit(ui_screen_t * screen) {
	app_sms_saved_sms = false;
	app_sms_count = 0;
	for (size_t i = 0; i < 3; i++) {
		if (app_sms_list[i]) {
			m26_sms_free(app_sms_list[i]);
		}
	}
}

static void app_sms_init() {
	/*
	 * main
	 */
	app_sms_main = ui_screen_new("SMS");

	app_sms_main->draw = &app_sms_draw;
	app_sms_main->deinit = &app_sms_main_deinit;
	app_sms_main->go_back = &app_sms_main_back;

	/*
	 * other
	 */
	app_sms_reply = ui_screen_new("Reply");

	ui_item_t * reply_list = ui_list_new(0, UI_STATUS_HEIGHT + UI_SCREEN_TITLE_HEIGHT, GRAPHICS_WIDTH, GRAPHICS_HEIGHT - (UI_STATUS_HEIGHT + UI_SCREEN_TITLE_HEIGHT));

	ui_list_metadata_t * reply_list_data = (ui_list_metadata_t *) reply_list->metadata;

	list_add(reply_list_data->items, ui_list_item_new("Hello", &icon_sms, &app_sms_reply_activate));
	list_add(reply_list_data->items, ui_list_item_new("Yes", &icon_sms, &app_sms_reply_activate));
	list_add(reply_list_data->items, ui_list_item_new("No", &icon_sms, &app_sms_reply_activate));
	list_add(reply_list_data->items, ui_list_item_new("OK", &icon_sms, &app_sms_reply_activate));

	app_sms_reply->list = (void *) reply_list;
	app_sms_reply->go_back = &app_sms_reply_back;
}

static void app_sms_start() {
	ui_set_screen(app_sms_main);
}

app_t app_sms = {
	.name = "SMS",

	.init = &app_sms_init,
	.start = &app_sms_start
};