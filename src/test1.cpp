#include <test1.hpp>
#include <Arduino.h>

void test1()
{
    Serial.begin(9600);
    byte b = 0xFF; // trying to convert to -1
    int i = int(b);
    int j = int(char(b)); // trying conversions

    Serial.println(i); // 255
    Serial.println(j); // -1
}