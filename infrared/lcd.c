#include "lcd.h"

#include <util/delay.h>

static void lcd_toggle_e()
{
	/* An instruction starts processing on a falling edge of the ENABLE bit */
	LCD_CONTROL_PORT |= (1<<LCD_CONTROL_E);
	_delay_ms(1);
	LCD_CONTROL_PORT &= ~(1<<LCD_CONTROL_E);
	_delay_ms(1);
}

static void reset_data_port()
{
	LCD_DATA_PORT &= LCD_INTERFACE_MASK;
}

static void send_data(char data)
{
#ifdef LCD_4_BIT_INTERFACE
	reset_data_port();
	LCD_DATA_PORT |= (data>>4) & 0x0F;
	lcd_toggle_e();
	reset_data_port();
	LCD_DATA_PORT |= data & 0x0F;
	lcd_toggle_e();
#else
	reset_data_port();
	LCD_DATA_PORT = data;
	lcd_toggle_e();
#endif
}

#define set_function_set(flags) send_data(LCD_CMD_FUNCTION_SET | flags)

#define set_display(flags) send_data(LCD_CMD_DISPLAY | flags)

#define set_shift(flags) send_data(LCD_CMD_SHIFT | flags)

#define set_entry_mode(flags) send_data(LCD_CMD_ENTRY_MODE_SET | flags)

#define set_display_clear() send_data(LCD_CMD_DISPLAY_CLEAR)

#define set_cursor_home() send_data(LCD_CMD_CURSOR_HOME)

static void set_ddram_address(char addr)
{
	LCD_CONTROL_PORT &= ~(1<<LCD_CONTROL_RS);
	send_data(LCD_CMD_DDRAM_ADDRESS | addr);
}

void lcd_init(char display_flags)
{
	/* The data port is set as output */
	LCD_DATA_PORT_DIR &= LCD_INTERFACE_MASK;
	LCD_DATA_PORT_DIR |= LCD_DATA_PORT_OUPUT & ~LCD_INTERFACE_MASK;

	/* and the control bits as well */
	LCD_CONTROL_PORT_DIR |= (1<<LCD_CONTROL_RS) | (1<<LCD_CONTROL_E);

	/* wait a little bit for the LCD to be ready */
	_delay_ms(20); /* datasheet specifies 10ms */

	LCD_CONTROL_PORT &= ~(1<<LCD_CONTROL_RS);

#ifdef LCD_4_BIT_INTERFACE
	reset_data_port();
	LCD_DATA_PORT |= LCD_CMD_FUNCTION_SET>>4;
	lcd_toggle_e();

	set_function_set(LCD_FLAG_FUNCTION_SET_N);
#else
	set_function_set(LCD_FLAG_FUNCTION_SET_DL | LCD_FLAG_FUNCTION_SET_N);
#endif


	set_display(LCD_FLAG_DISPLAY_D | display_flags);

	set_entry_mode(LCD_FLAG_ENTRY_MODE_INC);

	set_display_clear();

}

int lcd_putchar(char c, FILE *stream)
{
	if (c == '\n') {
		set_ddram_address(0x40);
		return 0;
	}

	LCD_DATA_PORT_DIR &= LCD_INTERFACE_MASK;
	LCD_DATA_PORT_DIR |= LCD_DATA_PORT_OUPUT & ~LCD_INTERFACE_MASK;

	LCD_CONTROL_PORT |= (1<<LCD_CONTROL_RS);

	send_data(c);
	return 0;
}

void lcd_home()
{
	set_ddram_address(0x00);
	set_display_clear();
	set_cursor_home();
}

