MODEL = atmega8
F_CPU=16000000UL

CFLAGS = -Os -Wall -I. -DF_CPU=$(F_CPU)
ASFLAGS = -Os -Wall -I.
FLOAT_FLAGS = -Wl,-u,vfprintf -lprintf_flt -lm 
LDFLAGS = -Wl,-Map,main.map
LIB_DIR = ../lib
INCLUDE_DIR = ../include

TARGET_ARCH = -mmcu=$(MODEL)
CC = avr-gcc
AR = avr-ar

# Device info
DEVICE=m8
PROGRAMMER=usbasp
PGRPORT=/dev/usbasp

# Fuses (XXX : 1 means NOT programmed)
# LFUSE : BODLEVEL | BODEN | SUT1  | SUT0  | CKSEL3 | CKSEL2  | CKSEL1  | CKSEL0

# 0xE1 (default) : no brown-out, 1MHz, slow rising power (65ms from reset).
#LFUSE=0xE1

# 8MHz
#LFUSE=0xE4

# Crystal oscillator 16Mhz
LFUSE=0xEF

# HFUSE : RSTDISBL | WDTON | SPIEN | CKOPT | EESAVE | BOOTSZ1 | BOOTSZ0 | BOOTRST

# 0xd9 (default) : reset pin enabled, WDT enabled by WDTCR, serial programming enabled,
# 					internal clock, EEPROM not preserved through Chip Erase, 
# 					boot size 1024 words, reset vector = application reset.
#HFUSE=0xD9

# Crystal oscillator 16Mhz
HFUSE=0xC9

