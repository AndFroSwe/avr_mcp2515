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
// Commands
#define MCP2515_WRITE 0x02 // Write instruction command
#define MCP2515_BIT_MOD 0x05 // Modify bit in register
#define MCP2515_RESET 0xC0 // Resets the state of mcp
#define MCP2515_MODE_CONFIG 0x80 // Configuration mode bits
#define MCP2515_MODE_NORMAL 0x00 // Normal mode
// Baud rate commands
#define MCP2515_CAN_100kBPS_CFG1 0x03
#define MCP2515_CAN_100kBPS_CFG2 0xFA
#define MCP2515_CAN_100kBPS_CFG3 0x87

// Registers addresses
#define MCP2515_MODE_REG 0x0F // Mode choice register
// Baud rate registers
#define MCP2515_CNF1_REG 0x2A
#define MCP2515_CNF2_REG 0x29
#define MCP2515_CNF3_REG 0x28

// Masks
#define MCP2515_MODE_MASK 0xE0

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
void mcp2515_init(io_pin cs);
void mcp2515_reset(io_pin cs);
void mcp2515_set_reg(uint8_t reg, uint8_t cmd, io_pin cs);
void mcp2515_bit_modify(uint8_t reg, uint8_t mask, uint8_t cmd, io_pin cs);
void mcp2515_setmode(const uint8_t mode, io_pin cs);
void mcp2515_set_baud(const uint8_t rate, io_pin cs);
