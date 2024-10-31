#include <Arduino.h>
#include <prog.hpp>

//#include <test4.hpp>

void setup()
{
    init_lcd();
    // test4();
}

void loop()
{
    pinMode(8, INPUT);
    pinMode(9, INPUT);
    pinMode(10, INPUT);
    get_lcd()->clear();
    get_lcd()->print("run: 1, 2 or 3");
    int lunch_prog = -1;
    while (lunch_prog == -1)
    {
        if (digitalRead(8))
            lunch_prog = 0;
        if (digitalRead(9))
            lunch_prog = 1;
        if (digitalRead(10))
            lunch_prog = 2;
    }
    delay(500);
    start_prog(lunch_prog);
    run_prog();
}
