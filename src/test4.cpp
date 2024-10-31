#include <prog.hpp>
#include <test4.hpp>
#include <EEPROM.h>

/* this file is to save the editor program in the eeprom because we can't write it directly because there isn't an editor */

void write_edit_prog();

#define SIZE_EDIT_PROG 128

byte edit_prog[SIZE_EDIT_PROG * 2] = {
    0xE0 + INPUT, 8, // pinmode(8,input)
    0xE0 + INPUT, 9,
    0xE0 + INPUT, 10,
    0xE0 + INPUT, 11,
    0X08, 124,  // num string line in reg0
    0xF0, 0x07, // lcd print string
/*<- 5*/
    0xD2, 8,   // digital read pin 8
    0xC5, 16, // jump if read (line 16)
    0xD2, 9,   // digital read pin 9
    0xC5, 14, // jump if read (line 14)
    0xD2, 10,  // digital read pin 10
    0xC6, 6, // jump if !read to digital read 8 (line 6)
    0x0A, 2,   // mov 2 in reg 2
    0xC0, 17, // jump (line 17)
    // read 9
    0x0A, 1,   // mov 1 in reg 2
    0xC0, 17, // jump (line 17)
    //read 8
    0x0A, 0,   // mov 0 in reg 2
/*<- 16*/
    0x08, 0x02, // MOV 2 dans register 0
    0x70, 254,  // ADD 254 to reg 0 = 256
    0x0B, 0,    // MOV 0 -> reg3
    0x94, 0x20, // MUL reg0 by reg2
    0x00, 128,  // MOV reg0 -> memory[128]
/*<- 21*/
    0xF0, 0x1B, // READ word EEPROM[reg0] -> reg1
    0x0E, 0X13, // MOV reg1 -> mem[reg3]
    0x70, 2,    // ADD 2 to reg0
    0x73, 1,    // ADD 1 to reg3
    0x0A, 128,  // MOV 128 -> reg2
    0x84, 0x32, // reg2 - reg3
    0xC2, 22,  // JNZ to (read word eeprom) (line 22)
/*<- 28*/
    0x08, 0,   // MOV 0 -> reg0
    0x09, 0,   // MOV 0 -> reg1
    0x0A, 0,   // MOV 0 -> reg2
    0x0B, 0,   // MOV 0 -> reg3
    0x00, 129, // MOV reg0 -> memory[129]
    0x00, 130, // MOV reg0 -> memory[130]
/*<- 34*/
    // printing
    0xF0, 0x1A, // lcd clear
    0x04, 129,  // MOV memory[129] -> reg0
    0x0D, 0x01, // MOV mem[reg0] -> reg1
    0xF0, 0x16, // print int reg0
    0x08, ' ',
    0xF0, 0x15, // print char reg0
    0x0C, 0x10, // mov reg1 -> reg0
    0xF0, 0x1D, // print hex reg0
    0x08, 0x01, // MOV 0x01 in reg0
    0xF0, 0x14, // print char reg0
    0x04, 130,  // MOV memory[130] -> reg0
    0xF0, 0x16, // print int reg0
    0x0C, 0x20, // mov reg2 -> reg0
    0xF0, 0x1D, // print hex reg0
/*<- 48*/
    0xD2, 8,   // digital read pin 8
    0xC5, 76, // jump if read (line 76)
    0xD2, 9,   // digital read pin 9
    0xC5, 90, // jump if read (line 90)
    0xD2, 11,  // digital read pin 11
    0xC5, 114, // jump if read (line 114)
    0xD2, 10,  // digital read pin 10
    0xC6, 49, // jump if !read to digital read 8 (line 49)
/*<- 56*/
    // read 10:
    0x34, 0x33, // reg3 = reg3 | reg3
    0xC1, 63,  // jump if zero to the next MOV memory[130] -> reg0 (line 63)
    // if !0:
    0x34, 0x22, // reg2 = reg2 | reg2
    0xC1, 49,  // jump if zero (to digital read 8) (line 49)
    // if !0:
    0x82, 1,   // sub 1 to reg2
    0xC0, 35, // jump to printing (MOV memory[129] -> reg0)
    0x04, 130, // MOV memory[130] -> reg0
    0x80, 3,   // sub 3 to reg0
    0xC1, 69, // jump if zero to the next MOV memory[129] -> reg0 (line 69)
    0x70, 4,   // add 4 to reg0
    0x00, 130, // MOV reg0 -> memory[130]
    0xC0, 35, // jump to printing (MOV memory[129] -> reg0)
    0x00, 130, // MOV reg0 -> memory[130]
    0x04, 129, // MOV memory[129] -> reg0
    0x80, 127, // sub 127 to reg0
    0xC1, 49, // jump if zero (to digital read 8)
    0x70, 128, // add 128 to reg0
    0x00, 129, // MOV reg0 -> memory[129]
    0xC0, 35, // jump to printing (MOV memory[129] -> reg0)
/*<- 75*/
    // read 8:
    0x34, 0x33, // reg3 = reg3 | reg3
    0xC1, 82,  // jump if zero to the next MOV memory[129] -> reg0 (line 82)
    // if !0:
    0x82, 15,  // sub 15 to reg2
    0xC1, 35, // jump if zero to printing (MOV memory[129] -> reg0)
    0x72, 16,  // add 16 to reg2
    0xC0, 35, // jump to printing (MOV memory[129] -> reg0)
    0x04, 129, // MOV memory[129] -> reg0
    0x80, 1,   // sub 1 to reg0
    0xC4, 86, // JUMP if positive to MOV reg0 -> memory[129] (line 86)
    0x08, 0,   // MOV 0 -> reg0
    0x00, 129, // MOV reg0 -> memory[129]
    0x08, 0,   // MOV 0 -> reg0
    0x00, 130, // MOV reg0 -> memory[130]
    0xC0, 35, // jump to printing (MOV memory[129] -> reg0)
/*<- 89*/
    // read 9:
    0x03, 131,  // MOV reg3 -> memory[131]
    0x08, 3,    // MOV 3 -> reg0 (this block here and not after to save 2 lines)
    0x07, 130,  // MOV memory[130] -> reg3
    0x84, 0x30, // reg0 = reg0 - reg3
    0x90, 4,    // MUL reg 0 by 4 <=> reg0
    0x07, 131,  // MOV memory[131] -> reg3
/*<- 95*/
    0x34, 0x33, // reg3 = reg3 | reg3
    0xC1,108,  // jump if zero to MOV reg1 -> reg2 (line 108)
    // if !0:
    0x0B, 0x0F, // MOV 0x0F -> reg3
    0x54, 0x03, // reg3 << reg0
    0x13, 0x00, // NOT reg3
    0x24, 0x31, // reg1 &= reg3
    0x54, 0x02, // reg2 << reg0
    0x34, 0x21, // reg1 |= reg2
    0x04, 129,  // MOV memory[129] -> reg0
    0x0E, 0x10, // MOV reg1 -> mem[reg0]
    0x0B, 0,    // MOV 0 -> reg3
    0xC0, 35,  // jump to printing (MOV memory[129] -> reg0)
    // if 0:
    0x0C, 0x12, // MOV reg1 -> reg2
    0x64, 0x02, // reg2 >> reg0
    0x0B, 0x0F, // MOV 0x0F -> reg3
    0x24, 0x32, // reg2 &= reg3
    0x0B, 1,    // MOV 1 -> reg3
    0xC0, 35,  // jump to printing (MOV memory[129] -> reg0)
/*<- 113*/
    // read 11:
    0x04, 128, // MOV memory[128] -> reg0
    0x0A, 0,   // MOV 0 -> reg2
/*<- 115*/
    0x0D, 0x21, // MOV mem[reg2] -> reg1
    0xF0, 0x1C, // write eeprom : eeprom[reg0] <- reg1
    0x70, 2,    // ADD 2 to reg0
    0x72, 1,    // ADD 1 to reg2
    0x0B, 128,  // MOV 128 -> reg3
    0x84, 0x23, // sub reg2 to reg3
    0xC2, 116,  // jump if not zero (to MOV mem[reg2] -> reg1) (line 115)
/*<- 122*/
    0xF0, 0x00, // END
    // DATA
    '1',' ', '2', ' ', '3','\0'};

void test4()
{
    write_edit_prog();
    init_lcd();
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

void write_edit_prog()
{
    for (int i = 0; i < SIZE_EDIT_PROG * 2; i++)
    {
        EEPROM.update(i, edit_prog[i]);
    }
}