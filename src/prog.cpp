#include <prog.hpp>
#include <EEPROM.h>
#include <debug.hpp>

union instruction
{
    uint16_t w;
    struct DB
    {
        byte h;
        byte l;
    } db;
};

#define MAX_INSTRUCTION_PER_PROG 128
instruction prog[MAX_INSTRUCTION_PER_PROG];
bool is_prog_running = 0;
byte prog_counter;
byte next_op;
byte flags;
#define FLAG_ZERO 0x80
#define FLAG_SIGN 0x40
#define FLAG_READ 0x20
void update_flags(long result);
int16_t regs[4];
int16_t memory[256];
LiquidCrystal *lcd;

void exec_inst(instruction inst);
void exec_mov(byte mod, byte par);
void exec_not(byte mod, byte par);
void exec_and(byte mod, byte par);
void exec_or(byte mod, byte par);
void exec_xor(byte mod, byte par);
void exec_shl(byte mod, byte par);
void exec_shr(byte mod, byte par);
void exec_add(byte mod, byte par);
void exec_sub(byte mod, byte par);
void exec_mul(byte mod, byte par);
void exec_div(byte mod, byte par);
void exec_mod(byte mod, byte par);
void exec_jmp(byte mod, byte par);
void exec_drw(byte mod, byte par);
void exec_pinmode(byte mod, byte par);
void exec_int(word int_num);
void exec_aread();  // ANALOG READ
void exec_awrite(); // ANALOG WRITE
void exec_delay();
void exec_eread();  // READ EEPROM
void exec_ewrite(); // WRITE EEPROM
void exec_sprints();
void exec_lcdprints();
void exec_sbegin(); // SERIAL BEGIN
void exec_send();   // SERIAL END
void exec_sprintc();
void exec_sprintint();
void exec_sprintlong();
void exec_sprintuint();
void exec_sprintulong();
void exec_micros();
void exec_millis();
void exec_sublong();
void exec_lcdbegin(); // lcd BEGIN
void exec_lcdend();
void exec_lcdsetcursor();
void exec_lcdprintc();
void exec_lcdprintint();
void exec_lcdprintlong();
void exec_lcdprintuint();
void exec_lcdprintulong();
void exec_lcdclear();
void exec_ewread();  // READ word EEPROM
void exec_ewwrite(); // WRITE word EEPROM
void exec_lcdprinthex();

void init_lcd()
{
    lcd = new LiquidCrystal(2, 3, 4, 5, 6, 7);
    lcd->begin(16, 2);
    lcd->clear();
}

LiquidCrystal *get_lcd() { return lcd; }

void start_prog(int num)
{
    if (num < 3 && num >= 0)
    {
        for (int i = 0; i < MAX_INSTRUCTION_PER_PROG; i++)
        {
            prog[i].w = word(EEPROM.read(i * 2 + 1 + (num*256)), EEPROM.read(i * 2 + (num*256))); // instructions are 2 bytes
        }
        is_prog_running = 1;
        prog_counter = 0;
        next_op = 1;
        flags = 0;
        lcd->clear();
    }
}

int run_prog()
{
    regs[0] = NO_ERR;
    while (is_prog_running)
    {
        exec_inst(prog[prog_counter]);
        prog_counter = next_op;
        next_op++;
    }
    DEBUG_PRINT("\nexit: %d\n",regs[0]);
    return regs[0];
}

byte get_flags()
{
    return flags;
}

void update_flags(long result)
{
    if (result == 0)
        flags |= byte(FLAG_ZERO);
    else
        flags &= byte(~FLAG_ZERO);
    if (result < 0)
        flags |= byte(FLAG_SIGN);
    else
        flags &= byte(~FLAG_SIGN);
}

void exec_inst(instruction inst)
{
    byte op = inst.db.h >> 4;
    byte mod = inst.db.h & 0x0F;
    byte par = inst.db.l;
    word int_num = word(inst.db.h & 0x0F,inst.db.l);
    switch (op)
    {
    case 0x0:
        exec_mov(mod, par);
        break;
    case 0x1:
        exec_not(mod, par);
        break;
    case 0x2:
        exec_and(mod, par);
        break;
    case 0x3:
        exec_or(mod, par);
        break;
    case 0x4:
        exec_xor(mod, par);
        break;
    case 0x5:
        exec_shl(mod, par);
        break;
    case 0x6:
        exec_shr(mod, par);
        break;
    case 0x7:
        exec_add(mod, par);
        break;
    case 0x8:
        exec_sub(mod, par);
        break;
    case 0x9:
        exec_mul(mod, par);
        break;
    case 0xA:
        exec_div(mod, par);
        break;
    case 0xB:
        exec_mod(mod, par);
        break;
    case 0xC:
        exec_jmp(mod, par);
        break;
    case 0xD:
        exec_drw(mod, par);
        break;
    case 0xE:
        exec_pinmode(mod, par);
        break;
    case 0xF:
        exec_int(int_num);
        break;
    default:
        is_prog_running = 0;
        regs[0] = ERR_OP;
        break;
    }
}

void exec_mov(byte mod, byte par)
{
    switch (mod)
    {
    case 0x0:
    case 0x1:
    case 0x2:
    case 0x3:
        DEBUG_PRINT("MOV: reg%d -> mem[%d] (reg%d = %d)\n", mod, par, mod, regs[mod]);
        memory[par] = regs[mod];
        break;
    case 0x4:
    case 0x5:
    case 0x6:
    case 0x7:
        DEBUG_PRINT("MOV: mem[%d] -> reg%d (mem[%d] = %d)\n", par, mod - 0x4, par, memory[par]);
        regs[mod - 0x4] = memory[par];
        break;
    case 0x8:
    case 0x9:
    case 0xA:
    case 0xB:
        DEBUG_PRINT("MOV: %d -> reg%d\n", par, mod - 0x8);
        regs[mod - 0x8] = int(par);
        break;
    case 0xC:
        DEBUG_PRINT("MOV: reg%d -> reg%d (reg%d = %d)\n", par >> 4, par & 0x0F, par >> 4, regs[par >> 4]);
        regs[par & 0x0F] = regs[par >> 4];
        break;
    case 0xD:
        DEBUG_PRINT("MOV: mem[reg%d] -> reg%d (mem[%d] = %d)\n", par >> 4, par & 0x0F, regs[par >> 4], memory[regs[par >> 4]]);
        regs[par & 0x0F] = memory[regs[par >> 4]];
        break;
    case 0xE:
        DEBUG_PRINT("MOV: reg%d -> mem[reg%d] (reg%d = %d & reg%d = %d)\n", par >> 4, par & 0x0F, par >> 4, regs[par >> 4], par & 0x0F, regs[par & 0x0F]);
        memory[regs[par & 0x0F]] = regs[par >> 4];
        break;
    default:
        is_prog_running = 0;
        regs[0] = ERR_MOD;
        break;
    }
}

void exec_not(byte mod, byte par)
{
    DEBUG_PRINT("NOT: reg%d = %d\n", mod, regs[mod]);
    regs[mod] = ~(regs[mod]);
    update_flags(regs[mod]);
}

void exec_and(byte mod, byte par)
{
    switch (mod)
    {
    case 0x0:
    case 0x1:
    case 0x2:
    case 0x3:
        DEBUG_PRINT("AND: reg%d &= %d (reg%d = %d)\n", mod, par, mod, regs[mod]);
        regs[mod] &= (0XFF00 + word(par));
        update_flags(regs[mod]);
        break;
    case 0x4:
        DEBUG_PRINT("AND: reg%d &= reg%d (reg%d = %d & reg%d = %d)\n", par & 0x0F, par >> 4, par & 0x0F, regs[par & 0x0F], par >> 4, regs[par >> 4]);
        regs[par & 0x0F] &= regs[par >> 4];
        update_flags(regs[par & 0x0F]);
        break;
    default:
        is_prog_running = 0;
        regs[0] = ERR_MOD;
        break;
    }
}

void exec_or(byte mod, byte par)
{
    switch (mod)
    {
    case 0x0:
    case 0x1:
    case 0x2:
    case 0x3:
        DEBUG_PRINT("OR: reg%d |= %d (reg%d = %d)\n", mod, par, mod, regs[mod]);
        regs[mod] |= word(par);
        update_flags(regs[mod]);
        break;
    case 0x4:
        DEBUG_PRINT("OR: reg%d |= reg%d (reg%d = %d & reg%d = %d)\n", par & 0x0F, par >> 4, par & 0x0F, regs[par & 0x0F], par >> 4, regs[par >> 4]);
        regs[par & 0x0F] |= regs[par >> 4];
        update_flags(regs[par & 0x0F]);
        break;
    default:
        is_prog_running = 0;
        regs[0] = ERR_MOD;
        break;
    }
}

void exec_xor(byte mod, byte par)
{
    switch (mod)
    {
    case 0x0:
    case 0x1:
    case 0x2:
    case 0x3:
        DEBUG_PRINT("XOR: reg%d ^= %d (reg%d = %d)\n", mod, par, mod, regs[mod]);
        regs[mod] ^= word(par);
        update_flags(regs[mod]);
        break;
    case 0x4:
        DEBUG_PRINT("XOR: reg%d ^= reg%d (reg%d = %d & reg%d = %d)\n", par & 0x0F, par >> 4, par & 0x0F, regs[par & 0x0F], par >> 4, regs[par >> 4]);
        regs[par & 0x0F] ^= regs[par >> 4];
        update_flags(regs[par & 0x0F]);
        break;
    default:
        is_prog_running = 0;
        regs[0] = ERR_MOD;
        break;
    }
}

void exec_shl(byte mod, byte par)
{
    switch (mod)
    {
    case 0x0:
    case 0x1:
    case 0x2:
    case 0x3:
        DEBUG_PRINT("SHL: reg%d << %d (reg%d = %d)\n", mod, par, mod, regs[mod]);
        regs[mod] = regs[mod] << par;
        update_flags(regs[mod]);
        break;
    case 0x4:
        DEBUG_PRINT("SHL: reg%d << reg%d (reg%d = %d & reg%d = %d)\n", par & 0x0F, par >> 4, par & 0x0F, regs[par & 0x0F], par >> 4, regs[par >> 4]);
        regs[par & 0x0F] = regs[par & 0x0F] << regs[par >> 4];
        break;
    default:
        is_prog_running = 0;
        regs[0] = ERR_MOD;
        break;
    }
}

void exec_shr(byte mod, byte par)
{
    switch (mod)
    {
    case 0x0:
    case 0x1:
    case 0x2:
    case 0x3:
        DEBUG_PRINT("SHR: reg%d >> %d (reg%d = %d)\n", mod, par, mod, regs[mod]);
        regs[mod] = regs[mod] >> par;
        update_flags(regs[mod]);
        break;
    case 0x4:
        DEBUG_PRINT("SHR: reg%d >> reg%d (reg%d = %d & reg%d = %d)\n", par & 0x0F, par >> 4, par & 0x0F, regs[par & 0x0F], par >> 4, regs[par >> 4]);
        regs[par & 0x0F] = regs[par & 0x0F] >> regs[par >> 4];
        break;
    default:
        is_prog_running = 0;
        regs[0] = ERR_MOD;
        break;
    }
}

void exec_add(byte mod, byte par)
{
    switch (mod)
    {
    case 0x0:
    case 0x1:
    case 0x2:
    case 0x3:
        DEBUG_PRINT("ADD: reg%d += %d (reg%d = %d)\n", mod, par, mod, regs[mod]);
        regs[mod] += int16_t(par);
        update_flags(regs[mod]);
        break;
    case 0x4:
        DEBUG_PRINT("ADD: reg%d += reg%d (reg%d = %d & reg%d = %d)\n", par & 0x0F, par >> 4, par & 0x0F, regs[par & 0x0F], par >> 4, regs[par >> 4]);
        regs[par & 0x0F] += regs[par >> 4];
        update_flags(regs[par & 0x0F]);
        break;
    default:
        is_prog_running = 0;
        regs[0] = ERR_MOD;
        break;
    }
}

void exec_sub(byte mod, byte par)
{
    switch (mod)
    {
    case 0x0:
    case 0x1:
    case 0x2:
    case 0x3:
        DEBUG_PRINT("SUB: reg%d -= %d (reg%d = %d)\n", mod, par, mod, regs[mod]);
        regs[mod] -= int16_t(par);
        update_flags(regs[mod]);
        break;
    case 0x4:
        DEBUG_PRINT("SUB: reg%d -= reg%d (reg%d = %d & reg%d = %d)\n", par & 0x0F, par >> 4, par & 0x0F, regs[par & 0x0F], par >> 4, regs[par >> 4]);
        regs[par & 0x0F] -= regs[par >> 4];
        update_flags(regs[par & 0x0F]);
        break;
    default:
        is_prog_running = 0;
        regs[0] = ERR_MOD;
        break;
    }
}

void exec_mul(byte mod, byte par)
{
    switch (mod)
    {
    case 0x0:
    case 0x1:
    case 0x2:
    case 0x3:
        DEBUG_PRINT("SUB: reg%d *= %d (reg%d = %d)\n", mod, par, mod, regs[mod]);
        regs[mod] *= int16_t(char(par));
        update_flags(regs[mod]);
        break;
    case 0x4:
        DEBUG_PRINT("MUL: reg%d *= reg%d (reg%d = %d & reg%d = %d)\n", par & 0x0F, par >> 4, par & 0x0F, regs[par & 0x0F], par >> 4, regs[par >> 4]);
        regs[par & 0x0F] *= regs[par >> 4];
        update_flags(regs[par & 0x0F]);
        break;
    default:
        is_prog_running = 0;
        regs[0] = ERR_MOD;
        break;
    }
}

void exec_div(byte mod, byte par)
{
    switch (mod)
    {
    case 0x0:
    case 0x1:
    case 0x2:
    case 0x3:
        DEBUG_PRINT("DIV: reg%d /= %d (reg%d = %d)\n", mod, par, mod, regs[mod]);
        regs[mod] /= int16_t(char(par));
        update_flags(regs[mod]);
        break;
    case 0x4:
        DEBUG_PRINT("DIV: reg%d /= reg%d (reg%d = %d & reg%d = %d)\n", par & 0x0F, par >> 4, par & 0x0F, regs[par & 0x0F], par >> 4, regs[par >> 4]);
        regs[par & 0x0F] /= regs[par >> 4];
        update_flags(regs[par & 0x0F]);
        break;
    default:
        is_prog_running = 0;
        regs[0] = ERR_MOD;
        break;
    }
}

void exec_mod(byte mod, byte par)
{
    switch (mod)
    {
    case 0x0:
    case 0x1:
    case 0x2:
    case 0x3:
        DEBUG_PRINT("MOD: reg%d %= %d (reg%d = %d)\n", mod, par, mod, regs[mod]);
        regs[mod] %= int16_t(char(par));
        update_flags(regs[mod]);
        break;
    case 0x4:
        DEBUG_PRINT("MOD: reg%d %= reg%d (reg%d = %d & reg%d = %d)\n", par & 0x0F, par >> 4, par & 0x0F, regs[par & 0x0F], par >> 4, regs[par >> 4]);
        regs[par & 0x0F] %= regs[par >> 4];
        update_flags(regs[par & 0x0F]);
        break;
    default:
        is_prog_running = 0;
        regs[0] = ERR_MOD;
        break;
    }
}

void exec_jmp(byte mod, byte par)
{
    bool do_jump = 0;
    switch (mod)
    {
    case 0x0:
        DEBUG_PRINT("JMP: -> %d ; ", par);
        do_jump = 1;
        break;
    case 0x1:
        DEBUG_PRINT("JZ: -> %d ; ", par);
        if (flags & FLAG_ZERO)
            do_jump = 1;
        break;
    case 0x2:
        DEBUG_PRINT("JNZ: -> %d ; ", par);
        if ((flags & FLAG_ZERO) == 0)
            do_jump = 1;
        break;
    case 0x3:
        DEBUG_PRINT("JS: -> %d ; ", par);
        if (flags & FLAG_SIGN)
            do_jump = 1;
        break;
    case 0x4:
        DEBUG_PRINT("JNS: -> %d ; ", par);
        if (!(flags & FLAG_SIGN))
            do_jump = 1;
        break;
    case 0x5:
        #if DEBUG
        #else
        delay(75);
        #endif
        DEBUG_PRINT("JR: -> %d ; ", par);
        if (flags & FLAG_READ)
            do_jump = 1;
        break;
    case 0x6:
        #if DEBUG
        #else
        delay(75);
        #endif
        DEBUG_PRINT("JNR: -> %d ; ", par);
        if ((flags & FLAG_READ) == 0)
            do_jump = 1;
        break;
    default:
        is_prog_running = 0;
        regs[0] = ERR_MOD;
        break;
    }
    if (do_jump)
    {
        DEBUG_PRINT("JUMP!\n",0);
        next_op = par;
    }
    else
    {
        DEBUG_PRINT("NO JUMP!\n",0);
    }
}

void exec_drw(byte mod, byte par)
{
    switch (mod)
    {
    case 0x0:
    case 0x1:
        DEBUG_PRINT("DWRITE %d to pin %d\n",mod,par);
        digitalWrite(par, mod);
        break;
    case 0x2:
        if (digitalRead(par))
        {
            DEBUG_PRINT("DREAD 1 in pin %d\n",par);
            flags |= byte(FLAG_READ);
        }
        else
        {
            DEBUG_PRINT("DREAD 0 in pin %d\n",par);
            flags &= byte(~FLAG_READ);
        }
        break;
    default:
        is_prog_running = 0;
        regs[0] = ERR_MOD;
        break;
    }
}

void exec_pinmode(byte mod, byte par)
{
    DEBUG_PRINT("PM %d pin %d\n",mod,par);
    pinMode(par, mod);
}

void exec_int(word int_num)
{
    switch (int_num)
    {
    case 0x000:
        is_prog_running = 0;
        regs[0] = NO_ERR;
        break;
    case 0x001:
        exec_aread();
        break;
    case 0x002:
        exec_awrite();
        break;
    case 0x003:
        exec_delay();
        break;
    case 0x004:
        exec_eread();
        break;
    case 0x005:
        exec_ewrite();
        break;
    case 0x006:
        exec_sprints();
        break;
    case 0x007:
        exec_lcdprints();
        break;
    case 0x008:
        exec_sbegin();
        break;
    case 0x009:
        exec_send();
        break;
    case 0x00A:
        exec_sprintc();
        break;
    case 0x00B:
        exec_sprintint();
        break;
    case 0x00C:
        exec_sprintlong();
        break;
    case 0x00D:
        exec_sprintuint();
        break;
    case 0x00E:
        exec_sprintulong();
        break;
    case 0x00F:
        exec_micros();
        break;
    case 0x010:
        exec_millis();
        break;
    case 0x011:
        exec_sublong();
        break;
    case 0x012:
        exec_lcdbegin();
        break;
    case 0x013:
        exec_lcdend();
        break;
    case 0x014:
        exec_lcdsetcursor();
        break;
    case 0x015:
        exec_lcdprintc();
        break;
    case 0x016:
        exec_lcdprintint();
        break;
    case 0x017:
        exec_lcdprintlong();
        break;
    case 0x018:
        exec_lcdprintuint();
        break;
    case 0x019:
        exec_lcdprintulong();
        break;
    case 0x01A:
        exec_lcdclear();
        break;
    case 0x01B:
        exec_ewread();
        break;
    case 0x01C:
        exec_ewwrite();
        break;
    case 0x01D:
        exec_lcdprinthex();
        break;
    default:
        is_prog_running = 0;
        regs[0] = ERR_INT_NUM;
        break;
    }
}

void exec_aread()
{
    regs[1] = analogRead(regs[0]);
}

void exec_awrite()
{
    analogWrite(regs[0], regs[1]);
}

void exec_delay()
{
    delay(regs[0]);
}

void exec_eread()
{
    regs[1] = eeprom_read_byte((uint8_t *)(regs[0]));
}

void exec_ewrite()
{
    eeprom_update_byte((uint8_t *)(regs[0]), regs[1] & 0x00FF);
}

void exec_sprints()
{
    Serial.print(String((char *)(&(prog[regs[0]]))));
}

void exec_lcdprints()
{
    lcd->print(String((char *)(&(prog[regs[0]]))));
}

void exec_sbegin()
{
    Serial.begin(regs[0]);
}

void exec_send()
{
    Serial.end();
}

void exec_sprintc()
{
    char c = char(regs[0]);
    Serial.print(c);
}

void exec_sprintint()
{
    int i = regs[0];
    Serial.print(i);
}

void exec_sprintlong()
{
    long l = ((long *)regs)[0];
    Serial.print(l);
}

void exec_sprintuint()
{
    unsigned int ui = ((unsigned int *)regs)[0];
    Serial.print(ui);
}

void exec_sprintulong()
{
    unsigned long ul = ((unsigned long *)regs)[0];
    Serial.print(ul);
}

void exec_micros()
{
    ((unsigned long *)regs)[0] = micros();
}

void exec_millis()
{
    ((unsigned long *)regs)[0] = millis();
}

void exec_sublong()
{
    ((long *)regs)[0] -= ((long *)regs)[1];
    update_flags(((long *)regs)[0]);
}

void exec_lcdbegin()
{
    lcd = new LiquidCrystal(regs[0] >> 8, regs[0] & 0x00FF, regs[1] >> 8, regs[1] & 0x00FF, regs[2] >> 8, regs[2] & 0x00FF);
    lcd->begin(16, 2);
}

void exec_lcdend()
{
    delete lcd;
}

void exec_lcdsetcursor()
{
    lcd->setCursor(regs[0] >> 4,regs[0] & 0x000F);
}

void exec_lcdprintc()
{
    char c = char(regs[0]);
    lcd->print(c);
}

void exec_lcdprintint()
{
    int i = regs[0];
    lcd->print(i);
}

void exec_lcdprintlong()
{
    long l = ((long *)regs)[0];
    lcd->print(l);
}

void exec_lcdprintuint()
{
    unsigned int ui = ((unsigned int *)regs)[0];
    lcd->print(ui);
}

void exec_lcdprintulong()
{
    unsigned long ul = ((unsigned long *)regs)[0];
    lcd->print(ul);
}

void exec_lcdclear()
{
    lcd->clear();
}

int16_t swapEndian(int16_t value) {
    uint16_t temp = static_cast<uint16_t>(value);
    temp = (temp >> 8) | (temp << 8);
    return static_cast<int16_t>(temp);
}

void exec_ewread()
{
    regs[1] = eeprom_read_word((uint16_t *)(regs[0]));
    regs[1] = swapEndian(regs[1]);
}

void exec_ewwrite()
{
    regs[1] = swapEndian(regs[1]);
    eeprom_update_word((uint16_t *)(regs[0]), regs[1]);
}

void exec_lcdprinthex()
{
    lcd->print(word(regs[0]), HEX);
}
