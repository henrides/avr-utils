/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <joerg@FreeBSD.ORG> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.        Joerg Wunsch
 * ----------------------------------------------------------------------------
 *
 * Stdio demo, upper layer of LCD driver.
 *
 * $Id$
 */

#include "defines.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <avr/io.h>

#include <util/delay.h>

#include "hd44780.h"
#include "lcd.h"

static int lcd_putchar(char c, FILE *unused);

static FILE lcd_stream = FDEV_SETUP_STREAM(lcd_putchar, NULL, _FDEV_SETUP_WRITE);
FILE *lcdout = &lcd_stream;

/*
 * Setup the LCD controller.  First, call the hardware initialization
 * function, then adjust the display attributes we want.
 */
void
lcd_init(void)
{

    hd44780_init();

    /*
     * Clear the display.
     */
    hd44780_outcmd(HD44780_CLR);
    hd44780_wait_ready(true);

    /*
     * Entry mode: auto-increment address counter, no display shift in
     * effect.
     */
    hd44780_outcmd(HD44780_ENTMODE(1, 0));
    hd44780_wait_ready(false);

    /*
     * Enable display, activate non-blinking cursor.
     */
    hd44780_outcmd(HD44780_DISPCTL(1, 1, 0));
    hd44780_wait_ready(false);
}

/*
 * Send character c to the LCD display.  After a '\n' has been seen,
 * the next character will first clear the display.
 */
static int
lcd_putchar(char c, FILE *unused)
{
    return lcd_putc(c);
}

int
lcd_putc(char c)
{
    static bool nl_seen;

    if (nl_seen && c != '\n')
    {
        /*
         * First character after newline, move on second line
         */
        hd44780_wait_ready(false);
        hd44780_outcmd(HD44780_DDADDR(0x40));

        nl_seen = false;
    }
    if (c == '\n')
    {
        nl_seen = true;
    }
    else
    {
        hd44780_wait_ready(false);
        hd44780_outdata(c);
    }

    return 0;
}

int
lcd_set_addr(uint8_t addr)
{
    hd44780_wait_ready(false);
    hd44780_outcmd(HD44780_DDADDR(addr));
    return 0;
}

int
lcd_clear()
{
    hd44780_wait_ready(false);
    hd44780_outcmd(HD44780_CLR);
    hd44780_wait_ready(false);
    hd44780_outcmd(HD44780_HOME);
    return 0;
}
