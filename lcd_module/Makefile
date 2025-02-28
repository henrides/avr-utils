OBJS=lcd.o
BIN=liblcd.a

include ../Makefile.inc

.PHONY: all clean install

all : $(BIN)

clean :
	rm -f $(BIN) $(OBJS)

$(BIN) : $(OBJS)
	$(AR) rcs $@ $(OBJS)

liblcd.a : lcd.h

install : $(BIN)
	mkdir -p $(LIB_DIR)
	cp $(BIN) $(LIB_DIR)
	mkdir -p $(INCLUDE_DIR)
	cp lcd.h $(INCLUDE_DIR)

