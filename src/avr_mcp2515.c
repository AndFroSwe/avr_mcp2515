/* 
 * File: avr_mcp2515.c
 * Author: andfro
 * Description: Library for MCP2515 CAN network controller
 */

#include "avr_mcp2515.h"

int main(int argc, char *argv[])
{
    // Start SPI
    spi_master_init();

    // Send as test
    char c = 0;
    while (1)
    {
        _delay_ms(100);
        spi_master_transmit(c++);
    }



    return 0;
}

void spi_master_init()
{
    // Set MOSI and SCK output, all others input
    DDRB |= _BV(SCK) | _BV(MOSI) | _BV(SS);
    DDRB &= ~_BV(MISO);

    // Enable SPI, Master, set clock rate fck/16
    SPCR |= _BV(SPE) | _BV(MSTR) | _BV(SPR0) |_BV(SPR1);
}

void spi_master_transmit(char c)
{
    // Start transmission
    SPDR = c;
    // Wait for transmission complete
    while(!(SPSR & _BV(SPIF)));
}
