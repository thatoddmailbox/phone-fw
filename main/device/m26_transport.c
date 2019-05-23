#include "device/m26_transport.h"

const char * M26_TRANSPORT_TAG = "m26_transport";

static int m26_transport_connect(esp_transport_handle_t t, const char * host, int port, int timeout_ms) {
	m26_tcp_open(host, port);
	return 0;
}

static int m26_transport_write(esp_transport_handle_t t, const char * buffer, int len, int timeout_ms) {
	return 0;
}

static int m26_transport_read(esp_transport_handle_t t, char * buffer, int len, int timeout_ms) {
	return 0;
}

static int m26_transport_poll_read(esp_transport_handle_t t, int timeout_ms) {
	return 0;
}

static int m26_transport_poll_write(esp_transport_handle_t t, int timeout_ms) {
	return 0;
}

static int m26_transport_close(esp_transport_handle_t t) {
	m26_tcp_close();
	return 0;
}

static esp_err_t m26_transport_destroy(esp_transport_handle_t t) {
	esp_transport_close(t);
	return 0;
}

esp_transport_handle_t m26_transport_init() {
	esp_transport_handle_t t = esp_transport_init();

	esp_transport_set_func(t, m26_transport_connect, m26_transport_read, m26_transport_write, m26_transport_close, m26_transport_poll_read, m26_transport_poll_write, m26_transport_destroy);

	return t;
}