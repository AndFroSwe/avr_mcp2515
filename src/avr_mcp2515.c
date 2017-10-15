/* 
 * File: avr_mcp2515.c
 * Author: andfro
 * Description: Library for MCP2515 CAN network controller
 */

#include "avr_mcp2515.h"


int main(int argc, char *argv[])
{
    // Set pins
    const io_pin spi_cs = {&PORTC, &DDRC, PC0};
    pinmode(spi_cs, OUTPUT);
    // For debugging
    const io_pin led1 = {&PORTD, &DDRD, PD0};
    const io_pin led2 = {&PORTD, &DDRD, PD1};
    pinmode(led1, OUTPUT);
    pinmode(led2, OUTPUT);
    digital_write(led1, LOW);
    digital_write(led2, LOW);

    // Start SPI
    spi_master_init();

    // Initialize CAN
    mcp2515_init(spi_cs);

    // Setup transmit buffer
    // Set interrupt policy
    mcp2515_bit_modify(MCP2515_CANINTE, 0x04, 0x00, spi_cs); // Set to not interrupt

    // Send as test
    uint8_t c = 0;
    while (1)
    {
        _delay_ms(1000);
        /*c = mcp2515_read_reg(MCP2515_MODE_REG, spi_cs);*/

        /*spi_master_readwrite(0x60);*/
        /*mcp2515_init(spi_cs);*/
        mcp2515_tx(c++, spi_cs);
    }



    return 0;
}

/* SPI functions */
void spi_master_init()
{
    // Set MOSI and SCK output, all others input
    DDRB |= _BV(SCK) | _BV(MOSI) | _BV(SS);
    DDRB &= ~_BV(MISO);

    // Enable SPI, Master, set clock rate fck/16
    SPCR |= _BV(SPE) | _BV(MSTR) | _BV(SPR0) |_BV(SPR1);
}

uint8_t spi_master_readwrite(uint8_t cmd)
{
    // Start transmission
    SPDR = cmd;
    // Wait for transmission complete
    while(!(SPSR & _BV(SPIF)));

    // Return read
    return SPDR;
}

/* Pin manipulation functions */
int pinmode(io_pin p, int s)
{
    if (s == INPUT)
    {
        *p.ddr &= ~_BV(p.pin);
        return 0;
    }
    else if (s == OUTPUT)
    {
        *p.ddr |= _BV(p.pin);
        return 0;
    }
    return -1; // Return fail if wrong input mode
}
int digital_write(io_pin p, int s)
{
    /* Set output pin high or low */
    if (s == LOW)
    {
        *p.port &= ~_BV(p.pin);
        return 0;
    }
    else if (s == HIGH)
    {
        *p.port |= _BV(p.pin);
        return 0;
    }
    return -1; // Return fail if wrong input mode
}

/* MCP2515 functions */
void mcp2515_init(io_pin cs)
{
    // Set up mcp
    mcp2515_reset(cs);
    mcp2515_setmode(MCP2515_MODE_CONFIG, cs);
    mcp2515_set_baud(1, cs); // 1 is dummy value for now
    // Init receive buffers
    // Init transmit buffers
    // Set interrupt mode
    mcp2515_setmode(MCP2515_MODE_NORMAL, cs);
}

void mcp2515_reset(io_pin cs)
{
    // Reset mcp
    digital_write(cs, LOW); // Start transaction
    spi_master_readwrite(MCP2515_RESET);
    digital_write(cs, HIGH);
}
void mcp2515_set_reg(uint8_t reg, uint8_t cmd, io_pin cs)
{
    // Write cmd to register reg
    digital_write(cs, LOW);
    _delay_us(CS_WAIT); // Wait for CS to settle
    spi_master_readwrite(MCP2515_WRITE); // Initiate a write
    spi_master_readwrite(reg); // Send register adress
    spi_master_readwrite(cmd); // Send register value to write
    digital_write(cs, HIGH); // Stop transaction
}


void mcp2515_bit_modify(uint8_t reg, uint8_t mask, uint8_t cmd, io_pin cs)
{
    // Modifies bit in register
    digital_write(cs, LOW);
    _delay_us(CS_WAIT); // Wait for CS to settle
    spi_master_readwrite(MCP2515_BIT_MOD); // Initiate a write
    spi_master_readwrite(reg); // Send register adress
    spi_master_readwrite(mask); // Send mask
    spi_master_readwrite(cmd); // Send register value to write
    digital_write(cs, HIGH);
}

void mcp2515_setmode(const uint8_t mode, io_pin cs)
{
    // Sets the mcp into chosen mode
    digital_write(cs, LOW);
    _delay_us(CS_WAIT);
    spi_master_readwrite(MCP2515_BIT_MOD);
    spi_master_readwrite(MCP2515_MODE_REG);
    spi_master_readwrite(MCP2515_MODE_MASK);
    spi_master_readwrite(mode);
    digital_write(cs, HIGH);
}

void mcp2515_set_baud(const uint8_t rate, io_pin cs)
{
    // Sets baud rate of CAN network
    // NOTE: This only sets one speed at this point
    digital_write(cs, LOW);
    _delay_us(CS_WAIT);
    mcp2515_set_reg(MCP2515_CNF1_REG, MCP2515_CAN_100kBPS_CFG1, cs);
    mcp2515_set_reg(MCP2515_CNF2_REG, MCP2515_CAN_100kBPS_CFG2, cs);
    mcp2515_set_reg(MCP2515_CNF3_REG, MCP2515_CAN_100kBPS_CFG3, cs);
    digital_write(cs, HIGH);
}

void mcp2515_tx(uint8_t msg, io_pin cs)
{
    // Send message on CAN
    // NOTE: This temporarily has hard coded parameters

    digital_write(cs, LOW);
    // Start write at buffer
    spi_master_readwrite(MCP2515_WRITE); // Write instruction
    // Control register
    spi_master_readwrite(MCP2515_TXB0CTRL); // First address in tx buffer
    spi_master_readwrite(MCP2515_PRIO_HIGHEST); // Message prio
    // Standard identifier
    spi_master_readwrite(0x00);
    spi_master_readwrite(0x20);
    // Extended identifier
    spi_master_readwrite(0x00);
    spi_master_readwrite(0x00);
    // DLD and RTR
    spi_master_readwrite(0x01); // Data frame, 1 byte
    // Data bytes
    spi_master_readwrite(msg); // Byte 0
    spi_master_readwrite(0x00); // Byte 1
    spi_master_readwrite(0x00); // Byte 2
    spi_master_readwrite(0x00); // Byte 3
    spi_master_readwrite(0x00); // Byte 4
    spi_master_readwrite(0x00); // Byte 5
    spi_master_readwrite(0x00); // Byte 6
    spi_master_readwrite(0x00); // Byte 7
    digital_write(cs, HIGH);

    // Send message
    mcp2515_rts(0x01, cs);
}

void mcp2515_rts(uint8_t buf, io_pin cs)
{
    // Sends a request to send for given buffer
    digital_write(cs, LOW);
    _delay_us(CS_WAIT);
    spi_master_readwrite((MCP2515_RTS | buf));
    digital_write(cs, HIGH);
}

uint8_t mcp2515_read_reg(uint8_t addr, io_pin cs)
{
    // Reads a register from MCP
    digital_write(cs, LOW);
    spi_master_readwrite(MCP2515_READ);
    spi_master_readwrite(addr);
    uint8_t reg = spi_master_readwrite(0x00);
    digital_write(cs, HIGH);

    return reg;
}
