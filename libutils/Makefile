OBJS=avr_io_pin.o link_list.o

LIB=libutils.a
HEADERS=avr_io_pin.h link_list.h

include ../Makefile.inc

.PHONY: all clean install

all : $(LIB)

clean :
	rm -f $(LIB) $(OBJS)

$(LIB) : $(OBJS)
	$(AR) rcs $@ $(OBJS)

libutils.a : $(HEADERS)

install : $(LIB)
	mkdir -p $(LIB_DIR)
	cp $(LIB) $(LIB_DIR)
	mkdir -p $(INCLUDE_DIR)
	cp $(HEADERS) $(INCLUDE_DIR)

