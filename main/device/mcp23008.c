#include "device/mcp23008.h"

void mcp23008_init() {
    i2c_config_t conf = {
    	.mode = I2C_MODE_MASTER,
    	.sda_io_num = MCP23008_SDA,
    	.sda_pullup_en = GPIO_PULLUP_ENABLE,
    	.scl_io_num = MCP23008_SCL,
    	.scl_pullup_en = GPIO_PULLUP_ENABLE,
    	.master.clk_speed = MCP23008_FREQ
	};
	i2c_param_config(MCP23008_I2C, &conf);

	i2c_driver_install(MCP23008_I2C, conf.mode, 0, 0, 0);

	// all inputs
	mcp23008_write_register(MCP23008_IODIR, 0b1111111);

	// enable pullups
	mcp23008_write_register(MCP23008_GPPU, 0b1111111);

	// enable interrupts whenever a pin flips
	mcp23008_write_register(MCP23008_GPINTEN, 0b1111111);
	mcp23008_write_register(MCP23008_INTCON, 0b00000000);
}

uint8_t mcp23008_read_register(uint8_t reg) {
	uint8_t value;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();

	i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MCP23008_ADDRESS | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
	i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MCP23008_ADDRESS | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, &value, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(MCP23008_I2C, cmd, 1000 / portTICK_RATE_MS);

	i2c_cmd_link_delete(cmd);
	ESP_ERROR_CHECK(ret);

	return value;
}

void mcp23008_write_register(uint8_t reg, uint8_t data) {
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MCP23008_ADDRESS | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_write_byte(cmd, data, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(MCP23008_I2C, cmd, 1000 / portTICK_RATE_MS);

	i2c_cmd_link_delete(cmd);
	ESP_ERROR_CHECK(ret);
}

void mcp23008_test() {
	
}