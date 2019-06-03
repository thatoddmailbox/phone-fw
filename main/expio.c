#include "expio.h"

uint8_t current_direction;
uint8_t current_pull_enabled;
uint8_t current_pull_direction;
uint8_t current_output;

void expio_init() {
    current_direction = 0b11111111;
    current_pull_enabled = 0b00000000;
    current_pull_direction = 0b11111111;
    current_output = 0b00000000;

	pcal6416_write_register(PCAL6416_CONFIG_1, current_direction);
	pcal6416_write_register(PCAL6416_PULL_ENABLE_1, current_pull_enabled);
	pcal6416_write_register(PCAL6416_PULL_SELECT_1, current_pull_direction);
}

void expio_set_direction(uint8_t pin, uint8_t direction) {
    if (direction) {
        current_direction = current_direction | (1 << pin);
    } else {
        current_direction = current_direction & ~(1 << pin);
    }
	pcal6416_write_register(PCAL6416_CONFIG_1, current_direction);
}

void expio_set_pullup(uint8_t pin, bool on) {
    if (on) {
        current_pull_enabled = current_pull_enabled | (1 << pin);
    } else {
        current_pull_enabled = current_pull_enabled & ~(1 << pin);
    }
	pcal6416_write_register(PCAL6416_PULL_ENABLE_1, current_pull_enabled);
}

uint8_t expio_get_level(uint8_t pin) {
    uint8_t current_state = pcal6416_read_register(PCAL6416_INPUT_1);
    if (current_state & (1 << pin)) {
        return 1;
    } else {
        return 0;
    }
}

void expio_set_level(uint8_t pin, uint8_t level) {
    if (level) {
        current_output = current_output | (1 << pin);
    } else {
        current_output = current_output & ~(1 << pin);
    }
	pcal6416_write_register(PCAL6416_OUTPUT_1, current_output);
}