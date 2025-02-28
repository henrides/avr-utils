OBJS=lcd.o hd44780.o
INCLUDES=-I$(INCLUDE_DIR)

LIB=liblcd.a
HEADERS=lcd.h

include ../Makefile.inc

.PHONY: all clean install

all : $(LIB)

clean :
	rm -f $(LIB) $(OBJS)

$(LIB) : $(OBJS)
	$(AR) rcs $@ $(OBJS)

%.o : %.c
	$(COMPILE.c) -o $@ $< $(INCLUDES)

libencoder.a : $(HEADERS)

install : $(LIB)
	mkdir -p $(LIB_DIR)
	cp $(LIB) $(LIB_DIR)
	mkdir -p $(INCLUDE_DIR)
	cp $(HEADERS) $(INCLUDE_DIR)

