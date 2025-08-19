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

uint8_t status_row_0 = 0;
uint8_t status_row_1 = 0;
uint8_t status_row_2 = 0;
uint8_t status_row_3 = 0;
uint8_t status_row_4 = 0;

uint8_t out_data_1 = 0;
uint8_t out_data_2 = 0;

void pulse(uint8_t pin)
{
    PORTB |= (1 << pin);
    _delay_us(5);
    PORTB &= ~(1 << pin);
    _delay_us(5);
}

void delay_5us()
{
    _delay_us(5);
}

void out_latch_up()
{
    PORTB |= (1 << RCLK);
}

void out_latch_down()
{
    PORTB &= ~(1 << RCLK);
}

void in_latch_up()
{
    PORTB |= (1 << SHLD);
}
void in_latch_down()

{
    PORTB &= ~(1 << SHLD);
}

void clock_up()
{
    PORTB |= (1 << SRCLK);
}

void clock_down()
{
    PORTB &= ~(1 << SRCLK);
}

void update_row_0()
{

    out_latch_down() for (int8_t i = 7; i >= 0; i--)
    {
        if (data & (1 << i))
            PORTB |= (1 << SER);
        else
            PORTB &= ~(1 << SER);
        pulse(SRCLK);
    }
    for (int8_t i = 7; i >= 0; i--)
    {
        if (data & (1 << i))
            PORTB |= (1 << SER);
        else
            PORTB &= ~(1 << SER);
        pulse(SRCLK);
    }
    shiftOut(b1);
    shiftOut(b2);
    out_latch_up()
}

void prepare_out_data(uint8_t row, uint8_t row_data)
{
    out_data_1 = 0x00;
    out_data_2 = 0x1F;
    if (row == 0)
        b2 -= 0x01;
    if (row == 1)
        b2 -= 0x02;
    if (row == 2)
        b2 -= 0x04;
    if (row == 3)
        b2 -= 0x08;
    if (row == 4)
        b2 -= 0x10;
    if (row_data && (1))
        b2 += 0x20;
    if (row_data && (1 << 1))
        b2 += 0x40;
    if (row_data && (1 << 2))
        b2 += 0x80;
    if (row_data && (1 << 3))
        b1 += 1;
    if (row_data && (1 << 4))
        b1 += 1;
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