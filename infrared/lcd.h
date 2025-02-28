#ifndef _LCD_H_
#define _LCD_H_

#include <stdio.h>
#include <avr/io.h>
#include "mcu_settings.h"

#define LCD_4_BIT_INTERFACE

#if (!defined(LCD_4_BIT_INTERFACE) && !defined(LCD_8_BIT_INTERFACE)) || \
	(defined(LCD_4_BIT_INTERFACE) && defined(LCD_8_BIT_INTERFACE))
#error "Please define LCD_4_BIT_INTERFACE *or* LCD_8_BIT_INTERFACE"
#endif

#define LCD_LINE					2
#define LCD_CHARACTER_PER_LINE		16

#define LCD_CONTROL_PORT 			PORTC
#define LCD_CONTROL_PORT_DIR 		DDRC

#define LCD_CONTROL_RS				PC0
#define LCD_CONTROL_E				PC1

#define LCD_DATA_PORT				PORTB
#define LCD_DATA_PORT_DIR			DDRB

#define LCD_DATA_PORT_INPUT			( 0x00 )
#define LCD_DATA_PORT_OUPUT			( 0xFF )

#define LCD_CMD_DISPLAY_CLEAR		( 0x01 )

#define LCD_CMD_CURSOR_HOME			( 0x02 )

#define	LCD_CMD_ENTRY_MODE_SET		( 0x04 )
#define LCD_FLAG_ENTRY_MODE_INC		( 0x02 )	/* Increment flag for Entry Mode Set command */
#define LCD_FLAG_ENTRY_MODE_S		( 0x01 )	/* Enable Display shift */

#define LCD_CMD_DISPLAY				( 0x08 )
#define LCD_FLAG_DISPLAY_D			( 0x04 )	/* Display on */
#define LCD_FLAG_DISPLAY_C			( 0x02 )	/* Cursor on */
#define LCD_FLAG_DISPLAY_B			( 0x01 )	/* Blinking on */
#define LCD_FLAG_DISPLAY_NONE		( 0x00 )	/* No display mode */

#define LCD_CMD_SHIFT				( 0x10 )
#define LCD_FLAG_SHIFT_DISPLAY		( 0x08 )	/* Shift display */
#define LCD_FLAG_SHIFT_RIGHT		( 0x04 )	/* Shift to the right */

#define LCD_CMD_FUNCTION_SET		( 0x20 )
#define LCD_FLAG_FUNCTION_SET_DL	( 0x10 )	/* Use 8-bit data port */
#define LCD_FLAG_FUNCTION_SET_N		( 0x08 )	/* Dual line LCD */

#define LCD_CMD_DDRAM_ADDRESS		( 0x80 )

#ifdef LCD_4_BIT_INTERFACE
#define LCD_INTERFACE_MASK			( 0xF0 )
#else
#define LCD_INTERFACE_MASK			( 0x00 )
#endif

void lcd_init(char display_flags);
int lcd_putchar(char c, FILE *stream);
void lcd_home();

#endif /* _LCD_H_ */
