#include "device/l80.h"

SemaphoreHandle_t l80_mutex = NULL;
StaticSemaphore_t l80_mutex_buffer;

static QueueHandle_t l80_uart_queue;

static char line_buffer[L80_LINE_BUFFER_SIZE];

static char rx_buffer[L80_RX_BUFFER_SIZE];
static size_t rx_buffer_length = 0;

const char * L80_TAG = "l80";

void l80_init() {
	// create static mutex
	l80_mutex = xSemaphoreCreateMutexStatic(&l80_mutex_buffer);
	configASSERT(l80_mutex);

	// set up uart
	uart_config_t l80_uart_config = {
		.baud_rate = 9600,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.rx_flow_ctrl_thresh = 122,
	};

	ESP_ERROR_CHECK(uart_param_config(L80_UART, &l80_uart_config));
	ESP_ERROR_CHECK(uart_set_pin(L80_UART, L80_RX, L80_TX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

	const int uart_buffer_size = (2 * 1024);

	ESP_ERROR_CHECK(uart_driver_install(L80_UART, uart_buffer_size, uart_buffer_size, 10, &l80_uart_queue, 0));
}

static size_t l80_get_line() {
	while (1) {
		// how many bytes are in the buffer?
		size_t length = 0;
		ESP_ERROR_CHECK(uart_get_buffered_data_len(L80_UART, &length));

		if (length == 0) {
			return 0;
		}

		// limit it to the space left in the rx buffer
		if (length > (L80_RX_BUFFER_SIZE - rx_buffer_length)) {
			length = (L80_RX_BUFFER_SIZE - rx_buffer_length);
		}
		length = uart_read_bytes(L80_UART, (uint8_t *) rx_buffer + rx_buffer_length, length, 100);

		rx_buffer_length += length;

		// now, scan through the buffer
		for (size_t i = 0; i < rx_buffer_length; i++) {
			if (rx_buffer[i] == '\n') {
				// found it

				// copy it out
				memcpy(line_buffer, rx_buffer, i);
				line_buffer[i] = '\0';

				// shift the rest of the buffer
				memcpy(rx_buffer, rx_buffer + i + 1, rx_buffer_length - i);
				rx_buffer_length -= (i + 1);

				return i - 1;
			}
		}
	}
}

char * l80_get_next_sentence() {
	size_t line_length = l80_get_line();
	if (line_length == 0) {
		return NULL;
	}
	return line_buffer;
}

void l80_wait_for_next_sentence() {
	uart_event_t event;

	while (1) {
		if (xQueueReceive(l80_uart_queue, (void *) &event, (portTickType) portMAX_DELAY)) {
			if (event.type == UART_DATA) {
				return;
			} else if (event.type == UART_BUFFER_FULL) {
				ESP_LOGE(L80_TAG, "buffer full");
				uart_flush_input(L80_UART);
				xQueueReset(l80_uart_queue);
				break;
			} else if (event.type == UART_FIFO_OVF) {
				ESP_LOGE(L80_TAG, "FIFO overflow");
				uart_flush_input(L80_UART);
				xQueueReset(l80_uart_queue);
				break;
			}
			ESP_LOGI(L80_TAG, "type %d", event.type);
		}
	}
}