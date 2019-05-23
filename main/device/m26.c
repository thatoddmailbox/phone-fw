#include "device/m26.h"

const char * M26_TAG = "m26";

const char * command_at = "AT\n";
const char * command_ate0 = "ATE0\n";
const char * command_at_ccid = "AT+CCID\n";
const char * command_at_cmee_2 = "AT+CMEE=2\n";
const char * command_at_cops = "AT+COPS?\n";
const char * command_at_gsn = "AT+GSN\n";
const char * command_at_qiact = "AT+QIACT\n";
const char * command_at_qiclose = "AT+QICLOSE\n";
const char * command_at_qideact = "AT+QIDEACT\n";
const char * command_at_qidnsip_1 = "AT+QIDNSIP=1\n";
const char * command_at_qifgcnt_0 = "AT+QIFGCNT=0\n";
const char * command_at_qilocip = "AT+QILOCIP\n";
const char * command_at_qimode_0 = "AT+QIMODE=0\n";
const char * command_at_qimux_0 = "AT+QIMUX=0\n";
const char * command_at_qiregapp = "AT+QIREGAPP\n";

char line_buffer[M26_LINE_BUFFER_SIZE];

char tx_buffer[M26_TX_BUFFER_SIZE];
char rx_buffer[M26_RX_BUFFER_SIZE];
size_t rx_buffer_length = 0;

void m26_init() {
	// set up pwrkey gpio
    gpio_pad_select_gpio(M26_PWRKEY);
	gpio_set_direction(M26_PWRKEY, GPIO_MODE_OUTPUT);
	gpio_set_level(M26_PWRKEY, 1);
	
	// set up uart
	const int uart_buffer_size = (1024 * 2);
	QueueHandle_t uart_queue;
	uart_config_t uart_config = {
		.baud_rate = 115200,
		.data_bits = UART_DATA_8_BITS,
		.parity = UART_PARITY_DISABLE,
		.stop_bits = UART_STOP_BITS_1,
		.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.rx_flow_ctrl_thresh = 122
	};

	ESP_ERROR_CHECK(uart_param_config(M26_UART, &uart_config));
	ESP_ERROR_CHECK(uart_set_pin(M26_UART, M26_RX, M26_TX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

	ESP_ERROR_CHECK(uart_driver_install(M26_UART, uart_buffer_size, uart_buffer_size, 10, &uart_queue, 0));

	// poke it
	ESP_LOGI(M26_TAG, "first try\n");
	for (size_t i = 0; i < 5; i++) {
		m26_send_command(command_at);
		m26_get_line(M26_DEFAULT_TIMEOUT); // AT
		m26_get_line(M26_DEFAULT_TIMEOUT); // OK
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}

	if (strlen(line_buffer) == 0) {
		// it didn't reply so probably it's off
		ESP_LOGI(M26_TAG, "it's off, toggling pwrkey\n");
		gpio_set_level(M26_PWRKEY, 0);
		vTaskDelay(1100 / portTICK_PERIOD_MS);
		gpio_set_level(M26_PWRKEY, 1);
		vTaskDelay(4000 / portTICK_PERIOD_MS);
		vTaskDelay(4000 / portTICK_PERIOD_MS);
	}

	// poke it again
	ESP_LOGI(M26_TAG, "second try\n");
	for (size_t i = 0; i < 5; i++) {
		m26_send_command(command_at);
		m26_get_line(M26_DEFAULT_TIMEOUT); // AT
		m26_get_line(M26_DEFAULT_TIMEOUT); // OK
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}

	// disable echo
	m26_send_command(command_ate0);
	vTaskDelay(10 / portTICK_PERIOD_MS);
	m26_get_line(M26_DEFAULT_TIMEOUT); // ATE0
	m26_get_line(M26_DEFAULT_TIMEOUT); // OK
	vTaskDelay(100 / portTICK_PERIOD_MS);
	
	// poke it yet again
	m26_send_command(command_at);
	vTaskDelay(10 / portTICK_PERIOD_MS);
	m26_get_line(M26_DEFAULT_TIMEOUT); // AT
	m26_get_line(M26_DEFAULT_TIMEOUT); // OK

	// ok now discard all of that
	vTaskDelay(100 / portTICK_PERIOD_MS);
	uart_flush_input(M26_UART);
	rx_buffer_length = 0;

	ESP_LOGI(M26_TAG, "it's on\n");

	// AT+CMEE=2 - verbose error
	m26_send_command(command_at_cmee_2);
	vTaskDelay(10 / portTICK_PERIOD_MS);
	m26_get_line(M26_DEFAULT_TIMEOUT); // blank
	m26_get_line(M26_DEFAULT_TIMEOUT); // OK

	// AT+QIMODE=0 - non-transparent mode for tcpip
	m26_send_command(command_at_qimode_0);
	vTaskDelay(10 / portTICK_PERIOD_MS);
	m26_get_line(M26_DEFAULT_TIMEOUT); // blank
	m26_get_line(M26_DEFAULT_TIMEOUT); // OK

	// AT+QIMUX=0 - turn off connection multiplexing
	m26_send_command(command_at_qimux_0);
	vTaskDelay(10 / portTICK_PERIOD_MS);
	m26_get_line(M26_DEFAULT_TIMEOUT); // blank
	m26_get_line(M26_DEFAULT_TIMEOUT); // OK

	// AT+QIFGCNT=0 - set context
	m26_send_command(command_at_qifgcnt_0);
	vTaskDelay(10 / portTICK_PERIOD_MS);
	m26_get_line(M26_DEFAULT_TIMEOUT); // blank
	m26_get_line(M26_DEFAULT_TIMEOUT); // OK
}

size_t m26_get_line(uint16_t timeout) {
	ESP_LOGI(M26_TAG, "m26_get_line");

	uint16_t time_waited = 0;
	while (time_waited < timeout) {
		// how many bytes are in the buffer?
		size_t length = 0;
		ESP_ERROR_CHECK(uart_get_buffered_data_len(M26_UART, &length));

		// limit it to the space left in the rx buffer
		if (length > (M26_RX_BUFFER_SIZE - rx_buffer_length)) {
			length = (M26_RX_BUFFER_SIZE - rx_buffer_length);
		}
		length = uart_read_bytes(M26_UART, (uint8_t *) rx_buffer + rx_buffer_length, length, 100);

		rx_buffer_length += length;
		
		// if (length > 0) {
		// 	ESP_LOGI(M26_TAG, "read %d bytes, new len %d", length, rx_buffer_length);
		// 	ESP_LOG_BUFFER_HEXDUMP(M26_TAG, rx_buffer, rx_buffer_length, ESP_LOG_INFO);
		// }

		// now, scan through the buffer
		for (size_t i = 0; i < rx_buffer_length; i++) {
			if (rx_buffer[i] == '\n') {
				// found it

				// ESP_LOGI(M26_TAG, "found new line at %d", i);

				// copy it out
				memcpy(line_buffer, rx_buffer, i);
				line_buffer[i] = '\0';

				// shift the rest of the buffer
				memcpy(rx_buffer, rx_buffer + i + 1, rx_buffer_length - i);
				rx_buffer_length -= (i + 1);

				// ESP_LOG_BUFFER_HEXDUMP(M26_TAG, rx_buffer, rx_buffer_length, ESP_LOG_INFO);
				ESP_LOGI(M26_TAG, "got line: %s", line_buffer);
				// ESP_LOGI(M26_TAG, "new len: %d", rx_buffer_length);
				// ESP_LOG_BUFFER_HEXDUMP(M26_TAG, rx_buffer, rx_buffer_length, ESP_LOG_INFO);

				return i - 1;
			}
		}		
		
		vTaskDelay(10 / portTICK_PERIOD_MS);
		time_waited += 10;
	}
	
	ESP_LOGI(M26_TAG, "m26_get_line timeout");

	return 0;
}

void m26_write_data(const char * data, size_t length) {
	uart_write_bytes(M26_UART, data, length);
	ESP_ERROR_CHECK(uart_wait_tx_done(M26_UART, 100));
}

void m26_send_command(const char * command) {
	ESP_LOGI(M26_TAG, "send: %s", command);
	m26_write_data(command, strlen(command));
}

void m26_send_single_reply(const char * command) {
	m26_send_command(command);
	m26_get_line(M26_DEFAULT_TIMEOUT); // blank
	m26_get_line(M26_DEFAULT_TIMEOUT); // response
}

void m26_get_ccid(char * ccid) {
	m26_send_command(command_at_ccid);
	vTaskDelay(10 / portTICK_PERIOD_MS);
	m26_get_line(M26_DEFAULT_TIMEOUT); // blank
	m26_get_line(M26_DEFAULT_TIMEOUT); // +CCID: "<ccid>""

	if (strlen(line_buffer) >= (strlen("+CCID: \"") + 20 + 1)) {
		memcpy(ccid, line_buffer + strlen("+CCID: \""), 20);
		ccid[20] = '\0';
	} else {
		ccid[0] = '\0';
	}

	m26_get_line(M26_DEFAULT_TIMEOUT); // blank
	m26_get_line(M26_DEFAULT_TIMEOUT); // OK
}

void m26_get_imei(char * imei) {
	m26_send_command(command_at_gsn);
	vTaskDelay(10 / portTICK_PERIOD_MS);
	m26_get_line(M26_DEFAULT_TIMEOUT); // blank
	m26_get_line(M26_DEFAULT_TIMEOUT); // <imei>

	if (strlen(line_buffer) > 15) {
		memcpy(imei, line_buffer, 15);
		imei[15] = '\0';
	} else {
		imei[0] = '\0';
	}

	m26_get_line(M26_DEFAULT_TIMEOUT); // blank
	m26_get_line(M26_DEFAULT_TIMEOUT); // OK
}

char * m26_get_operator() {
	m26_send_command(command_at_cops);
	vTaskDelay(10 / portTICK_PERIOD_MS);
	m26_get_line(M26_DEFAULT_TIMEOUT); // blank
	m26_get_line(M26_DEFAULT_TIMEOUT); // +COPS: 0,0,"<operator>"

	char * result = NULL;
	if (strlen(line_buffer) > strlen("+COPS: 0,0,\"\"")) {
		char * end_quote = strstr(line_buffer + strlen("+COPS: 0,0,\"") + 1, "\"");
		size_t length_to_end_quote = (size_t) (end_quote - line_buffer);
		size_t operator_name_length = length_to_end_quote - strlen("+COPS: 0,0,\"");

		result = malloc(operator_name_length + 1);
		memcpy(result, line_buffer + strlen("+COPS: 0,0,\""), operator_name_length);
		result[operator_name_length] = '\0';
	}

	m26_get_line(M26_DEFAULT_TIMEOUT); // blank
	m26_get_line(M26_DEFAULT_TIMEOUT); // OK

	return result;
}

void m26_gprs_activate(char * apn) {
	// set apn
	snprintf(tx_buffer, M26_TX_BUFFER_SIZE, "AT+QICSGP=1,\"%s\"\n", apn);
	m26_send_command(tx_buffer);
	vTaskDelay(10 / portTICK_PERIOD_MS);

	m26_get_line(M26_DEFAULT_TIMEOUT); // blank
	m26_get_line(M26_DEFAULT_TIMEOUT); // OK

	// start tcp/ip task
	m26_send_command(command_at_qiregapp);
	vTaskDelay(10 / portTICK_PERIOD_MS);

	m26_get_line(M26_DEFAULT_TIMEOUT); // blank
	m26_get_line(M26_DEFAULT_TIMEOUT); // OK

	// activate gprs context
	m26_send_command(command_at_qiact);
	vTaskDelay(10 / portTICK_PERIOD_MS);

	m26_get_line(5000); // blank
	m26_get_line(5000); // OK

	// wait a little
	vTaskDelay(300 / portTICK_PERIOD_MS);

	// query local ip (seems to be required for some reason?)
	m26_send_command(command_at_qilocip);
	vTaskDelay(10 / portTICK_PERIOD_MS);

	m26_get_line(M26_DEFAULT_TIMEOUT); // blank
	m26_get_line(M26_DEFAULT_TIMEOUT); // <ip address>
}

void m26_gprs_deactivate() {
	// deactivate gprs context
	m26_send_command(command_at_qideact);
	vTaskDelay(10 / portTICK_PERIOD_MS);

	m26_get_line(5000); // blank
	m26_get_line(5000); // OK
}

void m26_dns_set(char * primary, char * secondary) {
	// enable dns servers
	m26_send_command(command_at_qidnsip_1);
	vTaskDelay(10 / portTICK_PERIOD_MS);

	m26_get_line(M26_DEFAULT_TIMEOUT); // blank
	m26_get_line(M26_DEFAULT_TIMEOUT); // OK

	// set dns servers
	snprintf(tx_buffer, M26_TX_BUFFER_SIZE, "AT+QIDNSCFG=\"%s\",\"%s\"\n", primary, secondary);
	m26_send_command(tx_buffer);
	vTaskDelay(10 / portTICK_PERIOD_MS);

	m26_get_line(M26_DEFAULT_TIMEOUT); // blank
	m26_get_line(M26_DEFAULT_TIMEOUT); // OK
}

void m26_tcp_open(char * host, uint16_t port) {
	// open connection
	snprintf(tx_buffer, M26_TX_BUFFER_SIZE, "AT+QIOPEN=\"TCP\",\"%s\",\"%d\"\n", host, port);
	m26_send_command(tx_buffer);
	vTaskDelay(10 / portTICK_PERIOD_MS);

	m26_get_line(10000); // blank
	m26_get_line(10000); // OK
	m26_get_line(10000); // blank
	m26_get_line(10000); // CONNECT OK
}

void m26_tcp_write(const char * data, size_t length) {
	snprintf(tx_buffer, M26_TX_BUFFER_SIZE, "AT+QISEND=%d\n", length);
	m26_send_command(tx_buffer);
	vTaskDelay(10 / portTICK_PERIOD_MS);

	m26_get_line(M26_DEFAULT_TIMEOUT); // blank

	m26_write_data(data, length);

	m26_get_line(M26_DEFAULT_TIMEOUT); // >
	m26_get_line(M26_DEFAULT_TIMEOUT); // SEND OK
}

void m26_tcp_close() {
	m26_send_command(command_at_qiclose);
	vTaskDelay(10 / portTICK_PERIOD_MS);

	m26_get_line(M26_DEFAULT_TIMEOUT); // blank
	m26_get_line(M26_DEFAULT_TIMEOUT); // OK
}

void m26_http_get(char * url) {
	// set url
	snprintf(tx_buffer, M26_TX_BUFFER_SIZE, "AT+QHTTPURL=%d,60\n", strlen(url));

	m26_send_command(tx_buffer);
	vTaskDelay(10 / portTICK_PERIOD_MS);

	m26_get_line(M26_DEFAULT_TIMEOUT); // blank
	m26_get_line(M26_DEFAULT_TIMEOUT); // CONNECT

	m26_send_command(url);

	m26_get_line(M26_DEFAULT_TIMEOUT); // OK

	// make get request
	m26_send_command("AT+QHTTPGET=10\n");
	vTaskDelay(10 / portTICK_PERIOD_MS);

	m26_get_line(10000); // blank
	m26_get_line(10000); // OK

	// read response
	m26_send_command("AT+QHTTPREAD=10\n");
	vTaskDelay(10 / portTICK_PERIOD_MS);

	for (int i = 0; i < 10; i++) {
		m26_get_line(10000); // idk
		m26_get_line(10000); // idk
	}
}