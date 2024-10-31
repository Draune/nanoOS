#include <prog.hpp>
#include <test3.hpp>
#include <EEPROM.h>

void write_lcd_prog();

#define SIZE_LCD_PROG 36

byte lcd_prog[SIZE_LCD_PROG * 2] = {
    0xF0, 0x1A, // lcd clear
    0x08, 'h',  // MOV 'h' in register 0
    0xF0, 0x15, // lcd print char ('h')
    0x08, 'e',
    0xF0, 0x15,
    0x08, 'l',
    0xF0, 0x15,
    0x08, 'l',
    0xF0, 0x15,
    0x08, 'o',
    0xF0, 0x15,
    0x08, ' ',
    0xF0, 0x15,
    0x08, 'w',
    0xF0, 0x15,
    0x08, 'o',
    0xF0, 0x15,
    0x08, 'r',
    0xF0, 0x15,
    0x08, 'l',
    0xF0, 0x15,
    0x08, 'd',
    0xF0, 0x15, 
    0xF0, 0x13, // LCD END
    0xF0, 0x00  // END
};

void test3()
{
    write_lcd_prog();
    start_prog(0);
    Serial.begin(9600);
    Serial.print("start\n");
    Serial.end();
    int err = run_prog();
    Serial.begin(9600);
    Serial.print("\nend : ");
    Serial.print(err);
    Serial.end();
}

void write_lcd_prog()
{
    for (int i = 0; i < SIZE_LCD_PROG * 2; i++)
    {
        EEPROM.update(i, lcd_prog[i]);
    }
}