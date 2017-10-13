/* 
 * File: avr_mcp2515.c
 * Author: andfro
 * Description: Library for MCP2515 CAN network controller
 */

#include <avr/io.h>
#define F_CPU 8000000UL
#include <util/delay.h>

// Define SPI pins
#define SCK PB5
#define MISO PB4
#define MOSI PB3
#define SS PB2

// Function declarations
void spi_master_init();
void spi_master_transmit(char c);
