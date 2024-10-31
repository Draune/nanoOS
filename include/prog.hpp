#ifndef ARDUINO_PROJECT_PROG_H
#define ARDUINO_PROJECT_PROG_H

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Wire.h>

#define NO_ERR 0
#define ERR_OP 1
#define ERR_MOD 2
#define ERR_INT_NUM 3

void init_lcd();
LiquidCrystal* get_lcd();
void start_prog(int num);
int run_prog();

byte get_flags();

#endif