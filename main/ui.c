#include "ui.h"

const char * UI_TAG = "ui";

QueueHandle_t ui_queue;

void ui_init() {
	ui_queue = xQueueCreate(UI_EVENT_QUEUE_SIZE, sizeof(ui_event_t));
}

void ui_handle_event(ui_event_t event) {
	if (xQueueSendToBack(ui_queue, &event, 0) != pdTRUE) {
		ESP_LOGW(UI_TAG, "ui_handle_event: event dropped, couldn't add to queue!");
	}
}

void ui_step() {
	// update
	ui_event_t event_to_process;
	if (xQueueReceive(ui_queue, &event_to_process, 0)) {
		ESP_LOGI(UI_TAG, "got event");
		if (event_to_process.event_type == UI_EVENT_TYPE_BUTTON_UP) {
			ESP_LOGI(UI_TAG, "button up");
		}
	}

	// draw
}