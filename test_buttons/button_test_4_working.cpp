// test funzionante

#define F_CPU 1000000UL // Clock a 1MHz

#include <avr/io.h>
#include <util/delay.h>

#define QH PB0    // Data input from 74HC165
#define SER PB1   // Pin 6
#define SRCLK PB2 // Pin 7
#define RCLK PB3  // Pin 2
#define SHLD PB4  // Latch pin (SH/LD)

uint8_t out_data_1 = 0;
uint8_t out_data_2 = 0;

uint8_t row_0_status = 0;
uint8_t read_status = 0;

void pulse(uint8_t pin)
{
    PORTB |= (1 << pin);
    _delay_us(5);
    PORTB &= ~(1 << pin);
    _delay_us(5);
}

void pixel_on(uint8_t row, uint8_t row_data)
{
    out_data_1 = 0x00;
    out_data_2 = 0x1F;
    if (row == 0)
        out_data_2 -= 0x01;
    if (row == 1)
        out_data_2 -= 0x02;
    if (row == 2)
        out_data_2 -= 0x04;
    if (row == 3)
        out_data_2 -= 0x08;
    if (row == 4)
        out_data_2 -= 0x10;
    if (row_data & (1 << 0))
        out_data_2 += 0x20;
    if (row_data & (1 << 1))
        out_data_2 += 0x40;
    if (row_data & (1 << 2))
        out_data_2 += 0x80;
    if (row_data & (1 << 3))
        out_data_1 += 1;
    if (row_data & (1 << 4))
        out_data_1 += 2;
    out_data_1 += 0b1000;

    PORTB &= ~(1 << RCLK); // latch down
    for (int8_t i = 7; i >= 0; i--)
    {
        if (out_data_1 & (1 << i))
            PORTB |= (1 << SER);
        else
            PORTB &= ~(1 << SER);
        pulse(SRCLK);
    }
    for (int8_t i = 7; i >= 0; i--)
    {
        if (out_data_2 & (1 << i))
            PORTB |= (1 << SER);
        else
            PORTB &= ~(1 << SER);
        pulse(SRCLK);
    }
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
                pixel_on(row, 1 << col);
                _delay_ms(500);
            }
        }
    }

    return 0;
}