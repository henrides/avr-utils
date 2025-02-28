OBJS=test.o
NAME=test
BIN=$(NAME).hex
LIBS=-lpwr_latch -ltimer -lutils
LIBRARIES=-L$(LIB_DIR)
INCLUDES=-I$(INCLUDE_DIR)

include ../Makefile.inc

.PHONY : all clean install

all : $(BIN)

$(BIN) : $(NAME).elf
	avr-objcopy -j .text -j .data -O ihex $< $@
	avr-objcopy -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 -O ihex $< $(NAME)-eeprom.hex
	avr-nm -n $< > $(NAME).sym

$(NAME).elf : $(OBJS)
	$(LINK.o) -o $@ $(OBJS) $(LIBS) $(LIBRARIES)

%.o : %.c
	$(COMPILE.c) -o $@ $< $(INCLUDES)

clean :
	rm -f $(NAME).elf *.hex *.sym *.o *.map

size : $(NAME).elf
	avr-size --format=avr --mcu=$(MODEL) $(NAME).elf

install: $(BIN)
	avrdude -p $(DEVICE) -c $(PROGRAMMER) -P $(PGRPORT) -i 8 -U flash:w:$(BIN)

