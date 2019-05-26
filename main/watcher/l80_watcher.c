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
				sentence[strlen(sentence) - 1] = '\n';

				switch (minmea_sentence_id(sentence, false)) {
					case MINMEA_SENTENCE_RMC:
						{
							struct minmea_sentence_rmc frame;
							if (minmea_parse_rmc(&frame, sentence)) {
								ESP_LOGI(TAG_L80_WATCHER, "$RMC floating point degree coordinates and speed: (%f, %f) %f\n",
									minmea_tocoord(&frame.latitude),
									minmea_tocoord(&frame.longitude),
									minmea_tofloat(&frame.speed));
								ESP_LOGI(TAG_L80_WATCHER, "$RMC date and time: 20%d-%02d-%02d, %d:%02d:%02d\n",
									frame.date.year, frame.date.month, frame.date.day,
									frame.time.hours, frame.time.minutes, frame.time.seconds);
							}
						}
						break;

					case MINMEA_SENTENCE_GGA:
					case MINMEA_SENTENCE_GLL:
					case MINMEA_SENTENCE_GSA:
					case MINMEA_SENTENCE_GSV:
					case MINMEA_SENTENCE_VTG:
						break;

					default:
						ESP_LOGE(TAG_L80_WATCHER, "unknown sentence %s", sentence);
						break;
				}
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