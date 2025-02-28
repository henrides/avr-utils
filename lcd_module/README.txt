
[LCD Display]

-- LCD_8_BIT_INTERFACE --
Using 8-bit operation mode : atmega8 PortB is used as the lcd module data input.

Connections table:
+-------+-------+--------------------------------------------------------+
|  MCU  |  LCD  | Description                                            |
+-------+-------+--------------------------------------------------------+
|  PB0  |  DB0  | Data bit 0                                             |
|  PB1  |  DB1  | Data bit 1                                             |
|  PB2  |  DB2  | Data bit 2                                             |
|  PB3  |  DB3  | Data bit 3                                             |
|  PB4  |  DB4  | Data bit 4                                             |
|  PB5  |  DB5  | Data bit 5                                             |
|  PB6  |  DB6  | Data bit 6                                             |
|  PB7  |  DB7  | Data bit 7                                             |
+-------+-------+--------------------------------------------------------+
|  PC0  |  RS   | Register select : H = Data, L = Command                |
|  PC1  |  E    | Enable signal : Starts data read/write on falling edge |
+-------+-------+--------------------------------------------------------+

-- LCD_4_BIT_INTERFACE --
Using 4-bit operation mode : atmega8 lowest bits of PortB are used as the lcd module data input.

Connections table:
+-------+-------+--------------------------------------------------------+
|  MCU  |  LCD  | Description                                            |
+-------+-------+--------------------------------------------------------+
|  PB0  |  DB4  | Data bit 0                                             |
|  PB1  |  DB5  | Data bit 1                                             |
|  PB2  |  DB6  | Data bit 2                                             |
|  PB3  |  DB7  | Data bit 3                                             |
+-------+-------+--------------------------------------------------------+
|  PC0  |  RS   | Register select : H = Data, L = Command                |
|  PC1  |  E    | Enable signal : Starts data read/write on falling edge |
+-------+-------+--------------------------------------------------------+

Note : It loads the highest bits, triggers a write operation. Then it loads the lowest bits 
and triggers another write operation. It's a little bit slower but it uses less pins.

