#include "watcher/l80_watcher.h"

#define L80_WATCHER_TASK_STACK_SIZE 2*1024

const char * TAG_L80_WATCHER = "l80_watcher";

StaticTask_t l80_watcher_task_buffer;
StackType_t l80_watcher_task_stack[L80_WATCHER_TASK_STACK_SIZE];

TaskHandle_t l80_watcher_task_handle = NULL;

static void l80_watcher_task(void * pvParameters) {
	ESP_LOGI(TAG_L80_WATCHER, "Watcher task started");

	while (1) {
		xSemaphoreTake(l80_mutex, portMAX_DELAY);

		l80_wait_for_next_sentence();
		while (1) {
			char * sentence = l80_get_next_sentence();

			if (sentence) {
				ESP_LOGI(TAG_L80_WATCHER, "sentence: %s", sentence);
			} else {
				break;
			}
		}

		xSemaphoreGive(l80_mutex);
	}

	vTaskDelete(NULL);
}

void l80_watcher_start() {
	ESP_LOGI(TAG_L80_WATCHER, "Starting watcher task...");

	l80_watcher_task_handle = xTaskCreateStatic(
		l80_watcher_task,
		"L80 watcher",
		L80_WATCHER_TASK_STACK_SIZE,
		NULL,
		tskIDLE_PRIORITY,
		l80_watcher_task_stack,
		&l80_watcher_task_buffer
	);
}