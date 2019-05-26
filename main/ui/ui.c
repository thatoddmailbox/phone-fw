#include "ui/ui.h"

const char * UI_TAG = "ui";

ui_screen_t * ui_current_screen = NULL;
QueueHandle_t ui_event_queue;

bool ui_is_dirty = false;

void ui_init() {
	ui_event_queue = xQueueCreate(UI_EVENT_QUEUE_SIZE, sizeof(ui_event_t));
}

void ui_set_screen(ui_screen_t * screen) {
	ui_current_screen = screen;
	ui_is_dirty = true;
}

void ui_push_event(ui_event_t event) {
	if (xQueueSendToBack(ui_event_queue, &event, 0) != pdTRUE) {
		ESP_LOGW(UI_TAG, "ui_push_event: event dropped, couldn't add to queue!");
	}
}

void ui_step() {
	// update
	ui_event_t event_to_process;
	if (xQueueReceive(ui_event_queue, &event_to_process, 0)) {
		if (ui_current_screen) {
			ui_screen_process_event(ui_current_screen, &event_to_process);
		}
	}

	if (ui_current_screen) {
		ui_screen_update(ui_current_screen);
	}

	// draw
	if (ui_current_screen) {
		if (ui_is_dirty || ui_current_screen->force_redraw || ui_status_dirty()) {
			ui_screen_draw(ui_current_screen);
			graphics_flip();

			ui_current_screen->force_redraw = false;
			ui_is_dirty = false;
		}
	}
}