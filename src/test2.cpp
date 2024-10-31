#include <test2.hpp>
#include <prog.hpp>
#include <EEPROM.h>

void write_simple_prog();

#define SIZE_SIMPLE_PROG 11

byte simple_prog[SIZE_SIMPLE_PROG * 2] = { // should print : ccc
    0x08, 96,   // MOV 96 to reg 0
    0x90, 100,  // MUL reg 0 per 100
    0xF0, 0x08, // serial begin
    0x08, 'c',  // MOV 'c' to reg 0
    0x09, 'c'*2,  // MOV 'c'*2 to reg 1
    0x84, 0x10, // SUB reg 0 to reg 1
    0xF0, 0x0A, // serial print char ('c')
    0xC1, 0x05, // JZ to 6th instruction (SUB)
    0xC4, 0x05, // JNS to 6th instruction (SUB)
    0xF0, 0x09, // serial end
    0xF0, 0x00  // end
};

void test2()
{
    write_simple_prog();
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

void write_simple_prog()
{
    for (int i = 0; i < SIZE_SIMPLE_PROG * 2; i++)
    {
        EEPROM.update(i, simple_prog[i]);
    }
}