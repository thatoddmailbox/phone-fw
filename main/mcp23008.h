#ifndef _MCP23008_H
#define _MCP23008_H

#include <stdint.h>

#include <driver/i2c.h>

#define MCP23008_ADDRESS 0b01000000

#define MCP23008_IODIR 0x00
#define MCP23008_IPOL 0x01
#define MCP23008_GPINTEN 0x02
#define MCP23008_DEFVAL 0x03
#define MCP23008_INTCON 0x04
#define MCP23008_IOCON 0x05
#define MCP23008_GPPU 0x06
#define MCP23008_INTF 0x07
#define MCP23008_INTCAP 0x08
#define MCP23008_GPIO 0x09
#define MCP23008_OLAT 0x0A

#define MCP23008_FREQ 100000
#define MCP23008_I2C I2C_NUM_0
#define MCP23008_SCL 25
#define MCP23008_SDA 26

void mcp23008_init();

uint8_t mcp23008_read_register(uint8_t reg);
void mcp23008_write_register(uint8_t reg, uint8_t data);

void mcp23008_test();

#endif