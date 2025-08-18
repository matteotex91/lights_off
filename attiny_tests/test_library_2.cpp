
#include <TinyWireM.h> // Libreria per I2C su ATtiny85

// Definizione dei pin per i 74HC595
#define DATA_PIN 6  // SER - Data
#define CLOCK_PIN 7 // SRCLK - Clock
#define LATCH_PIN 2 // RCLK - Latch

void setup()
{
    TinyWireM.begin(); // Inizializza I²C

    // Configura i pin del 74HC595
    pinMode(DATA_PIN, OUTPUT);
    pinMode(CLOCK_PIN, OUTPUT);
    pinMode(LATCH_PIN, OUTPUT);
}

void pixel_on(uint8_t row, uint8_t col)
{
    uint8_t b1 = 0xFF;
    uint8_t b2 = 0xE0;
    if (row == 0)
        b2 -= 0x20;
    else if (row == 1)
        b2 -= 0x40;
    else if (row == 2)
        b2 -= 0x80;
    else if (row == 3)
        b1 -= 0x40;
    else
        b1 -= 0x80;
    if (col == 0)
        b2 += 0x10;
    else if (col == 1)
        b2 += 8;
    else if (col == 2)
        b2 += 4;
    else if (col == 3)
        b2 += 2;
    else
        b2 += 1;

    digitalWrite(LATCH_PIN, LOW); // Disattiva latch

    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, b1); // Secondo registro
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, b2); // Primo registro

    digitalWrite(LATCH_PIN, HIGH);
}

void loop()
{
    for (uint8_t row = 0; row < 5; row++)
    {
        for (uint8_t col = 0; col < 5; col++)
        {
            pixel_on(row, col);
            // delay(1);
        }
    }
}