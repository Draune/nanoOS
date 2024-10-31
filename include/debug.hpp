#ifndef DRAUNE_ARDUINO_DEBUG
#define DRAUNE_ARDUINO_DEBUG

#include <Arduino.h>
#include <stdarg.h>

#define DEBUG 0

#if DEBUG

#include <avr/pgmspace.h>

void debug_print(const char *format, ...) {
  Serial.begin(9600);

  char buffer[200]; // Taille du buffer à ajuster selon vos besoins
  va_list args;
  va_start(args, format);

  // Utilisation de sprintf_P pour formater la chaîne dans le buffer en RAM
  vsprintf_P(buffer, format, args);

  // Affichage du contenu du buffer
  Serial.print(buffer);

  va_end(args);

  Serial.end();
}

#define DEBUG_PRINT(fmt,...) do { \
  debug_print(PSTR(fmt), ##__VA_ARGS__); \
} while(0);

#else
#define DEBUG_PRINT(...) \
    do                   \
    {                    \
    } while (0)
#endif

#endif