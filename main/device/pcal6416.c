#include "device/pcal6416.h"

void pcal6416_init() {
    i2c_config_t conf = {
    	.mode = I2C_MODE_MASTER,
    	.sda_io_num = PCAL6416_SDA,
    	.sda_pullup_en = GPIO_PULLUP_ENABLE,
    	.scl_io_num = PCAL6416_SCL,
    	.scl_pullup_en = GPIO_PULLUP_ENABLE,
    	.master.clk_speed = PCAL6416_FREQ
	};
	i2c_param_config(PCAL6416_I2C, &conf);

	i2c_driver_install(PCAL6416_I2C, conf.mode, 0, 0, 0);
}

uint8_t pcal6416_read_register(uint8_t reg) {
	uint8_t value;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();

	i2c_master_start(cmd);
    i2c_master_write_byte(cmd, PCAL6416_ADDRESS | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
	i2c_master_start(cmd);
    i2c_master_write_byte(cmd, PCAL6416_ADDRESS | I2C_MASTER_READ, true);
    i2c_master_read_byte(cmd, &value, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(PCAL6416_I2C, cmd, 1000 / portTICK_RATE_MS);

	i2c_cmd_link_delete(cmd);
	ESP_ERROR_CHECK(ret);

	return value;
}

void pcal6416_write_register(uint8_t reg, uint8_t data) {
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, PCAL6416_ADDRESS | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_write_byte(cmd, data, true);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(PCAL6416_I2C, cmd, 1000 / portTICK_RATE_MS);

	i2c_cmd_link_delete(cmd);
	ESP_ERROR_CHECK(ret);
}

void pcal6416_test() {
	
}