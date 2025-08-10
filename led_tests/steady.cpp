#define F_CPU 1000000UL // Clock a 1MHz

#include <avr/io.h>
#include <util/delay.h>

#define SER PB1   // Pin 6
#define SRCLK PB2 // Pin 7
#define RCLK PB3  // Pin 2

void pulse(uint8_t pin)
{
    PORTB |= (1 << pin);
    _delay_us(100);
    PORTB &= ~(1 << pin);
    _delay_us(100);
}

void shiftOut(uint8_t data)
{
    for (int8_t i = 7; i >= 0; i--)
    {
        if (data & (1 << i))
            PORTB |= (1 << SER);
        else
            PORTB &= ~(1 << SER);
        pulse(SRCLK);
    }
}

void latch()
{
    pulse(RCLK);
}

void pixel_on(uint8_t row, uint8_t col)
{
    uint8_t b1 = 0xFF;
    uint8_t b2 = 0xE0;
    if (row == 0)
    {
        b2 -= 0x20;
    }
    else if (row == 1)
    {
        b2 -= 0x40;
    }
    else if (row == 2)
    {
        b2 -= 0x80;
    }
    else if (row == 3)
    {
        b1 -= 0x40;
    }
    else
    {
        b1 -= 0x80;
    }
    if (col == 0)
    {
        b2 += 0x10;
    }
    else if (col == 1)
    {
        b2 += 8;
    }
    else if (col == 2)
    {
        b2 += 4;
    }
    else if (col == 3)
    {
        b2 += 2;
    }
    else
    {
        b2 += 1;
    }
    shiftOut(0);
    shiftOut(0);
    latch();

    shiftOut(b1);
    shiftOut(b2);
    latch();
    _delay_ms(20);
}

int main()
{
    // Imposta pin come output
    DDRB |= (1 << SER) | (1 << SRCLK) | (1 << RCLK);

    while (1)
    {
        pixel_on(0, 0);
    }

    return 0;
}