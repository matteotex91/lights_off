#define F_CPU 1000000UL // Clock a 1MHz

#include <avr/io.h>
#include <util/delay.h>

#define SER PB1   // Pin 6
#define SRCLK PB2 // Pin 7
#define RCLK PB3  // Pin 2

void pulse(uint8_t pin)
{
    PORTB |= (1 << pin);
    _delay_us(5);
    PORTB &= ~(1 << pin);
    _delay_us(5);
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

void pixel_on(uint8_t row, uint8_t col)
{
    uint8_t b1 = 0x00;
    uint8_t b2 = 0x1F;
    if (row == 0)
        b2 -= 0x01;
    else if (row == 1)
        b2 -= 0x02;
    else if (row == 2)
        b2 -= 0x04;
    else if (row == 3)
        b2 -= 0x08;
    else
        b2 -= 0x10;
    if (col == 0)
        b2 += 0x20;
    else if (col == 1)
        b2 += 0x40;
    else if (col == 2)
        b2 += 0x80;
    else if (col == 3)
        b1 += 2;
    else
        b1 += 1;

    PORTB &= ~(1 << RCLK); // latch down
    shiftOut(b1);
    shiftOut(b2);
    PORTB |= (1 << RCLK); // latch up
}

int main()
{
    // Imposta pin come output
    DDRB |= (1 << SER) | (1 << SRCLK) | (1 << RCLK);

    while (1)
    {
        for (uint8_t row = 0; row < 5; row++)
        {
            for (uint8_t col = 0; col < 5; col++)
            {
                pixel_on(row, col);
                _delay_ms(100);
            }
        }
    }

    return 0;
}