// only led on, on the next test also test the reading

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

void out_signal_up()
{
    PORTB |= (1 << SER);
}

void out_signal_down()
{
    PORTB &= ~(1 << SER);
}

void clock_pulse()
{
    clock_up();
    delay_5us();
    clock_down();
    delay_5us();
}

void update_row_0()
{
    out_latch_down();
    for (int8_t i = 7; i >= 0; i--)
    {
        if (out_data_1 & (1 << i))
            out_latch_up();
        else
            out_latch_down();
        clock_pulse();
    }
    for (int8_t i = 7; i >= 0; i--)
    {
        if (out_data_2 & (1 << i))
            out_latch_up();
        else
            out_signal_down();
        clock_pulse();
    }
    out_latch_up();
}

void prepare_out_data(uint8_t row, uint8_t row_data)
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
    if (row_data & (1))
        out_data_2 += 0x20;
    if (row_data & (1 << 1))
        out_data_2 += 0x40;
    if (row_data & (1 << 2))
        out_data_2 += 0x80;
    if (row_data & (1 << 3))
        out_data_1 += 1;
    if (row_data & (1 << 4))
        out_data_1 += 2;
    out_data_1 += 4; // give voltage to the last button row
}

int main()
{

    // Imposta pin come output
    DDRB |= (1 << SER) | (1 << SRCLK) | (1 << RCLK);
    while (1)
    {
        prepare_out_data(0, 0b11111);
        _delay_us(100000);
    }

    return 0;
}