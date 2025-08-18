// ho fatto un altro errore, non ne faccio una giusta a
//  per il test 8 : attivare il latch del 595 e solo dopo, leggere i bottoni

#define F_CPU 1000000UL // Clock a 1MHz

#include <avr/io.h>
#include <util/delay.h>

#define QH PB0    // Data input from 74HC165
#define SER PB1   // Pin 6
#define SRCLK PB2 // Pin 7
#define RCLK PB3  // Pin 2
#define SHLD PB4  // Latch pin (SH/LD)

void pulse(uint8_t pin)
{
    PORTB |= (1 << pin);
    _delay_us(5);
    PORTB &= ~(1 << pin);
    _delay_us(5);
}

uint8_t shiftOut(uint8_t data1, uint8_t data2)
{
    uint8_t data = 0;
    uint8_t found = 0;
    PORTB &= ~(1 << RCLK); // latch down

    // abilitazione per lettura
    PORTB &= ~(1 << SHLD);
    _delay_us(5);
    PORTB |= (1 << SHLD);
    _delay_us(5);
    for (int8_t i = 7; i >= 0; i--)
    {
        if (data1 & (1 << i))
            PORTB |= (1 << SER);
        else
            PORTB &= ~(1 << SER);
        PORTB |= (1 << SRCLK);
        _delay_us(5);
        PORTB &= ~(1 << SRCLK);
        _delay_us(5);

        // lettura
        if (PINB & (1 << QH))
        {
            data = i;
            found = 1;
        }

        _delay_us(5);
    }
    PORTB |= (1 << RCLK);  // latch up
    PORTB &= ~(1 << RCLK); // latch down
    for (int8_t i = 7; i >= 0; i--)
    {
        if (data2 & (1 << i))
            PORTB |= (1 << SER);
        else
            PORTB &= ~(1 << SER);
        PORTB |= (1 << SRCLK);
        _delay_us(5);
        PORTB &= ~(1 << SRCLK);
        _delay_us(5);
    }
    PORTB |= (1 << RCLK); // latch up
    if (found)
    {
        return data;
    }
    else
    {
        return 6;
    }
}

uint8_t pixel_on(uint8_t row, uint8_t col)
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
    else if (row == 4)
        b2 -= 0x10;
    else
    {
    }
    if (col == 0)
        b2 += 0x20;
    else if (col == 1)
        b2 += 0x40;
    else if (col == 2)
        b2 += 0x80;
    else if (col == 3)
        b1 += 1;
    else if (col == 4)
        b1 += 2;
    else
    {
    }
    return shiftOut(b1, b2);
}

int main()
{

    // Imposta pin come output
    DDRB |= (1 << SER) | (1 << SRCLK) | (1 << RCLK);
    uint8_t data = 0;
    uint8_t row = 6;
    uint8_t col = 6;
    while (1)
    {
        for (uint8_t row = 0; row < 5; row++)
        {
            for (uint8_t col = 0; col < 5; col++)
            {
                data = pixel_on(row, col);
                _delay_us(500000);
            }
        }
    }

    return 0;
}