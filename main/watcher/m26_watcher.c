#include "watcher/m26_watcher.h"

#define M26_WATCHER_TASK_STACK_SIZE 2*1024

const char * TAG_M26_WATCHER = "m26_watcher";

StaticTask_t m26_watcher_task_buffer;
StackType_t m26_watcher_task_stack[M26_WATCHER_TASK_STACK_SIZE];

TaskHandle_t m26_watcher_task_handle = NULL;

static void m26_watcher_task(void * pvParameters) {
	ESP_LOGI(TAG_M26_WATCHER, "Watcher task started");
	while (1) {
		xSemaphoreTake(m26_mutex, portMAX_DELAY);

		uint8_t creg = m26_get_creg();
		ESP_LOGI(TAG_M26_WATCHER, "creg %d", creg);

		uint8_t csq = m26_get_csq();
		ESP_LOGI(TAG_M26_WATCHER, "csq %d", csq);

		atomic_store(&m26_watcher_dirty, true);
		atomic_store(&m26_watcher_registration, creg);
		atomic_store(&m26_watcher_signal, csq);

		// m26_gprs_activate("hologram");
		// m26_dns_set("8.8.8.8", "8.8.8.4");

		// // m26_http_get("http://aserv-cloud.cloudapp.net/phone/ping.php");
		// // m26_tcp_open("studerfamily.us", 9999);
		// // m26_tcp_write("Yay it works!\n", 14);
		// // m26_tcp_close();

		// m26_gprs_deactivate();

		xSemaphoreGive(m26_mutex);

		vTaskDelay(10000 / portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
}

void m26_watcher_start() {
	atomic_init(&m26_watcher_dirty, false);
	atomic_init(&m26_watcher_registration, M26_CREG_NOT_REGISTERED_SEARCHING);
	atomic_init(&m26_watcher_signal, 99);

	ESP_LOGI(TAG_M26_WATCHER, "Starting watcher task...");

	m26_watcher_task_handle = xTaskCreateStatic(
		m26_watcher_task,
		"M26 watcher",
		M26_WATCHER_TASK_STACK_SIZE,
		NULL,
		tskIDLE_PRIORITY,
		m26_watcher_task_stack,
		&m26_watcher_task_buffer
	);
}