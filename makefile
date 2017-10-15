CC = avr-gcc
CFLAGS = -Wall -O2 -fdiagnostics-color=always
OBJCOPY = avr-objcopy
MCU = atmega328p

# Compile
all: build/avr_mcp2515.hex flash

build/avr_mcp2515.hex: obj/avr_mcp2515.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

obj/avr_mcp2515.elf: obj/avr_mcp2515.o
	$(CC) $(CFLAGS) -mmcu=$(MCU) -o $@ $^ 

obj/avr_mcp2515.o: src/avr_mcp2515.c src/avr_mcp2515.h
	$(CC) $(CFLAGS) -mmcu=$(MCU) -c -o $@ $< 

# Flash
PART = m328p
PROGRAMMER = atmelice_isp

flash:
	avrdude -c $(PROGRAMMER) -p $(PART) -U flash:w:build/avr_mcp2515.hex

# Utils
clean:
	rm ./obj/*.o ./obj/*.elf ./build/*.hex -rf

.PHONY: all clean flash
