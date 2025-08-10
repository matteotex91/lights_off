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

void latch()
{
    pulse(RCLK);
}

int main()
{
    // Imposta pin come output
    DDRB |= (1 << SER) | (1 << SRCLK) | (1 << RCLK);

    // Sequenze di 8 bit da inviare
    const uint8_t patterns1[] = {
        00100110,
    };

    const uint8_t patterns2[] = {
        00000011,
    };

    while (1)
    {
        for (uint8_t i = 0; i < sizeof(patterns1); i++)
        {
            shiftOut(patterns2[i]);
            shiftOut(patterns1[i]);
            latch();
            _delay_ms(500);
        }
    }

    return 0;
}