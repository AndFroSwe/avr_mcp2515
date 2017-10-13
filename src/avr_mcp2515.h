/* 
 * File: avr_mcp2515.c
 * Author: andfro
 * Description: Library for MCP2515 CAN network controller
 */

/* Includes */
#include <avr/io.h>
#define F_CPU 8000000UL
#include <util/delay.h>

/* Defines */
// SPI
#define SCK PB5
#define MISO PB4
#define MOSI PB3
#define SS PB2

// Pins
#define LOW 0
#define HIGH 1
#define INPUT 0
#define OUTPUT 1

// MCP2515
#define CS_WAIT 10 // Delay time in us for CS to settle
#define MCP2515_WRITE 0x02 // Write instruction command
#define MCP2515_BIT_MOD 0x03 // Modify bit in register
#define MCP2515_RESET 0x60 // Resets the state of mcp

/* Typedefs */
typedef struct Io_pin {
    // Struct to make it easy to manipulate pins
    volatile uint8_t *port; // Pin state port
    volatile uint8_t *ddr;  // Pin direction register
    int pin;                // Pin name
} io_pin;


/* Functions declarations */
// SPI
void spi_master_init();
uint8_t spi_master_readwrite(uint8_t msg); // Reads and write via SPI


// Pins
int pinmode(io_pin p, int s); // Set pin to be output of input
int digital_write(io_pin p, int s); // Set digital pin. 0 for low, 1 for high

// MCP2515
void mcp2515_reset(io_pin cs);
void mcp2515_write_reg(uint8_t reg, uint8_t cmd, io_pin cs);
void mcp2515_bit_modify(uint8_t reg, uint8_t mask, uint8_t cmd, io_pin cs);
