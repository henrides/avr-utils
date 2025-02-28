MODEL = atmega8
TARGET_ARCH = -mmcu=$(MODEL)
CC = avr-gcc
CFLAGS = -Os -g -Wall -I. 
ASFLAGS = -Os -g -Wall -I.
LDFLAGS = -g -Wl,-Map,main.map

OBJS=main.o
BIN=flashing
ELF=$(BIN).elf
HEX=$(BIN).hex

# Device info
DEVICE=m8
PROGRAMMER=usbasp
PGRPORT=/dev/USBasp

# Fuses (XXX : 1 means NOT programmed)
# LFUSE : BODLEVEL | BODEN | SUT1  | SUT0  | CKSEL3 | CKSEL2  | CKSEL1  | CKSEL0
# 0xe1 (default) : no brown-out, 1MHz, slow rising power (65ms from reset).
LFUSE=0b11100100	# 8MHz
# HFUSE : RSTDISBL | WDTON | SPIEN | CKOPT | EESAVE | BOOTSZ1 | BOOTSZ0 | BOOTRST
# 0xd9 (default) : reset pin enabled, WDT enabled by WDTCR, serial programming enabled,
# 					internal clock, EEPROM not preserved through Chip Erase, 
# 					boot size 1024 words, reset vector = application reset.
HFUSE=0b11011001

.PHONY: all clean

all : $(HEX)

clean :
	rm -f $(ELF) *.hex *.sym *.o *.map

$(ELF) : $(OBJS)
	$(LINK.o) -o $@ $(OBJS)

$(HEX) : $(ELF)
	avr-objcopy -j .text -j .data -O ihex $(ELF) $(HEX)
	avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O ihex $(ELF) $(BIN)-eeprom.hex
	avr-nm -n $(ELF) > $(BIN).sym

flash : $(HEX)
	sudo avrdude -p $(DEVICE) -c $(PROGRAMMER) -P $(PGRPORT) -i 8 -U flash:w:$(HEX)

eeprom : $(BIN)-eeprom.hex
	sudo avrdude -p $(DEVICE) -c $(PROGRAMMER) -P $(PGRPORT) -i 8 -U eeprom:w:$(BIN)-eeprom.hex

fuse :
	sudo avrdude -p $(DEVICE) -c $(PROGRAMMER) -P $(PGRPORT) -i 8 -U lfuse:w:$(LFUSE):m
	sudo avrdude -p $(DEVICE) -c $(PROGRAMMER) -P $(PGRPORT) -i 8 -U hfuse:w:$(HFUSE):m

size : $(ELF)
	avr-size --format=avr --mcu=$(MODEL) $(ELF)

