#include "device/st7735s.h"

const char * ST7735S_TAG = "st7735s";

spi_device_handle_t st7735s_spi;

uint16_t st7735s_buffer[ST7735S_WIDTH * ST7735S_HEIGHT];

void st7735s_init() {
	ESP_LOGI(ST7735S_TAG, "st7735s_init");

	/*
	 * spi setup
	 */
    spi_bus_config_t bus_cfg = {
        .miso_io_num = -1,
        .mosi_io_num = ST7735S_SDA,
        .sclk_io_num = ST7735S_SCL,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 38416 // should be 128*128*2?
    };
    spi_device_interface_config_t dev_cfg = {
        .clock_speed_hz = 26*1000*1000,
        .mode = 0,
        .spics_io_num = -1,
        .queue_size = 7
	};

    ESP_ERROR_CHECK(spi_bus_initialize(HSPI_HOST, &bus_cfg, 1));
    ESP_ERROR_CHECK(spi_bus_add_device(HSPI_HOST, &dev_cfg, &st7735s_spi));

	ESP_LOGI(ST7735S_TAG, "spi done");

	/*
	 * gpio setup
	 */
    gpio_pad_select_gpio(ST7735S_CS);
    gpio_pad_select_gpio(ST7735S_DC);
	gpio_set_direction(ST7735S_CS, GPIO_MODE_OUTPUT);
	gpio_set_direction(ST7735S_DC, GPIO_MODE_OUTPUT);

#if HW_VERSION == EVAL_HW
    gpio_pad_select_gpio(ST7735S_BL);
	gpio_set_direction(ST7735S_BL, GPIO_MODE_OUTPUT);
	gpio_set_level(ST7735S_BL, 0);
#else
	expio_set_direction(EXPIO_LCD_BL, EXPIO_OUTPUT);
	expio_set_level(EXPIO_LCD_BL, 1);
#endif

	ESP_LOGI(ST7735S_TAG, "gpio done");

	/*
	 * lcd setup
	 * magic init incantations done according to eastrising
	 */

	// something something sleep
	st7735s_write_command(0x11);
	vTaskDelay(120 / portTICK_PERIOD_MS);

	// frame rate
	st7735s_write_command(0xb1);
	st7735s_write_data(0x02);
	st7735s_write_data(0x35);
	st7735s_write_data(0x36);
	
	st7735s_write_command(0xb2);
	st7735s_write_data(0x02);
	st7735s_write_data(0x35);
	st7735s_write_data(0x36);
	
	st7735s_write_command(0xb3);
	st7735s_write_data(0x02);
	st7735s_write_data(0x35);
	st7735s_write_data(0x36);
	st7735s_write_data(0x02);
	st7735s_write_data(0x35);
	st7735s_write_data(0x36);

	// column inversion
	st7735s_write_command(0xb4);
	st7735s_write_data(0x07);

	// power sequence
	st7735s_write_command(0xc0);
	st7735s_write_data(0xa2);
	st7735s_write_data(0x02);
	st7735s_write_data(0x84);
	
	st7735s_write_command(0xc1);
	st7735s_write_data(0x45);
	
	st7735s_write_command(0xc2);
	st7735s_write_data(0x0a);
	st7735s_write_data(0x00);
	
	st7735s_write_command(0xc3);
	st7735s_write_data(0x8a);
	st7735s_write_data(0x2a);
	
	st7735s_write_command(0xc4);
	st7735s_write_data(0x8a);
	st7735s_write_data(0xee);

	// vcom	
	st7735s_write_command(0xc5);
	st7735s_write_data(0x03);
	
	st7735s_write_command(0x36);
	st7735s_write_data(0xC8);

	// gamma sequence
	st7735s_write_command(0xe0);
	st7735s_write_data(0x12);
	st7735s_write_data(0x1c);
	st7735s_write_data(0x10);
	st7735s_write_data(0x18);
	st7735s_write_data(0x33);
	st7735s_write_data(0x2c);
	st7735s_write_data(0x25);
	st7735s_write_data(0x28);
	st7735s_write_data(0x28);
	st7735s_write_data(0x27);
	st7735s_write_data(0x2f);
	st7735s_write_data(0x3c);
	st7735s_write_data(0x00);
	st7735s_write_data(0x03);
	st7735s_write_data(0x03);
	st7735s_write_data(0x10);
	
	st7735s_write_command(0xe1);
	st7735s_write_data(0x12);
	st7735s_write_data(0x1c);
	st7735s_write_data(0x10);
	st7735s_write_data(0x18);
	st7735s_write_data(0x2d);
	st7735s_write_data(0x28);
	st7735s_write_data(0x23);
	st7735s_write_data(0x28);
	st7735s_write_data(0x28);
	st7735s_write_data(0x26);
	st7735s_write_data(0x2f);
	st7735s_write_data(0x3d);
	st7735s_write_data(0x00);
	st7735s_write_data(0x03);
	st7735s_write_data(0x03);
	st7735s_write_data(0x10);

	// 65k color mode
	st7735s_write_command(0x3A);
	st7735s_write_data(0x05);

	// display on
	st7735s_write_command(0x29);
	st7735s_write_command(0x2C);

	// set stuff up
	st7735s_set_window(0, 127, 0, 127);
	memset(st7735s_buffer, 0xFF, ST7735S_WIDTH * ST7735S_HEIGHT * 2);
}

void st7735s_write_command(uint8_t command) {
	gpio_set_level(ST7735S_CS, 0);
	gpio_set_level(ST7735S_DC, 0);

	char transmission[1];
	transmission[0] = command;
	spi_transaction_t t;
	memset(&t, 0, sizeof(t));
	t.length = 8;
	t.tx_buffer = transmission;
	ESP_ERROR_CHECK(spi_device_polling_transmit(st7735s_spi, &t));

	gpio_set_level(ST7735S_CS, 1);
}

void st7735s_write_data(uint8_t data) {
	gpio_set_level(ST7735S_CS, 0);
	gpio_set_level(ST7735S_DC, 1);

	char transmission[1];
	transmission[0] = data;
	spi_transaction_t t;
	memset(&t, 0, sizeof(t));
	t.length = 8;
	t.tx_buffer = transmission;
	ESP_ERROR_CHECK(spi_device_polling_transmit(st7735s_spi, &t));

	gpio_set_level(ST7735S_CS, 1);
}

void st7735s_write_data_16(uint16_t data) {
	gpio_set_level(ST7735S_CS, 0);
	gpio_set_level(ST7735S_DC, 1);

	char transmission[2];
	transmission[0] = (uint8_t) (data >> 8);
	transmission[1] = (uint8_t) data & 0xFF;
	spi_transaction_t t;
	memset(&t, 0, sizeof(t));
	t.length = 16;
	t.tx_buffer = transmission;
	ESP_ERROR_CHECK(spi_device_polling_transmit(st7735s_spi, &t));

	gpio_set_level(ST7735S_CS, 1);
}

void st7735s_set_window(uint8_t x0, uint8_t x1, uint8_t y0, uint8_t y1) {
	x0+=2;x1+=2;y0+=3;y1+=3;

 	st7735s_write_command(0x2A);
	st7735s_write_data(0x00);
	st7735s_write_data(x0);
	st7735s_write_data(0x00);
	st7735s_write_data(x1);
	st7735s_write_command(0x2B);
	st7735s_write_data(0x00);
	st7735s_write_data(y0);
	st7735s_write_data(0x00);
	st7735s_write_data(y1);
}

void st7735s_flip(uint16_t buffer[ST7735S_WIDTH * ST7735S_HEIGHT]) {
	st7735s_write_command(0x2C);

	gpio_set_level(ST7735S_CS, 0);
	gpio_set_level(ST7735S_DC, 1);

	spi_transaction_t t;
	memset(&t, 0, sizeof(t));
	t.length = ST7735S_WIDTH * ST7735S_HEIGHT * 16;
	t.tx_buffer = buffer;
	ESP_ERROR_CHECK(spi_device_polling_transmit(st7735s_spi, &t));

	gpio_set_level(ST7735S_CS, 1);

	memcpy(st7735s_buffer, buffer, ST7735S_WIDTH * ST7735S_HEIGHT * 2);
}