#include <prog.hpp>
#include <test5.hpp>
#include <EEPROM.h>

void write_prog();

#define SIZE_PROG 128

byte test_prog[SIZE_PROG * 2] = {
    0x08,2 ,// mov 2 in reg0
    

    0x80, 2,   // sub 2 to reg0
    0xC2, 0, // jnz (line 0)
    
    0x08, 6,
    0xF0,0x07,

    0xF0, 0x00, // END
    // DATA
    ' ', '1', ' ', '2', '\0', '3'};

void test5()
{
    write_prog();
    init_lcd();
    start_prog(0);
    Serial.begin(9600);
    Serial.print("start\n");
    Serial.end();
    int err = run_prog();
    Serial.begin(9600);
    Serial.print("\nend : ");
    Serial.print(err);
    Serial.print(get_flags());
    Serial.end();
}

void write_prog()
{
    for (int i = 0; i < SIZE_PROG * 2; i++)
    {
        EEPROM.update(i, test_prog[i]);
    }
}