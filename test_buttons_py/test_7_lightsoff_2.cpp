#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

#define QH PB0
#define SER PB1
#define CLK PB2
#define RCLK PB3
#define SHLD PB4

#define SET_HIGH(port, bit) ((port) |= (1 << (bit)))
#define SET_LOW(port, bit) ((port) &= ~(1 << (bit)))
#define READ_PIN(pin, bit) ((pin) & (1 << (bit)))

static inline void pulse_clk(void)
{
    SET_HIGH(PORTB, CLK);
    _delay_us(2);
    SET_LOW(PORTB, CLK);
    _delay_us(2);
}

void write_74hc595(uint16_t value, uint8_t bits = 16)
{
    for (int8_t i = bits - 1; i >= 0; --i)
    {
        if (value & (1u << i))
            SET_HIGH(PORTB, SER);
        else
            SET_LOW(PORTB, SER);
        pulse_clk();
    }
    SET_HIGH(PORTB, RCLK);
    _delay_us(2);
    SET_LOW(PORTB, RCLK);
}

uint8_t read_74hc165(void)
{
    uint8_t value = 0;
    SET_LOW(PORTB, SHLD);
    _delay_us(5);
    SET_HIGH(PORTB, SHLD);
    _delay_us(5);

    for (uint8_t i = 0; i < 8; ++i)
    {
        value <<= 1;
        if (READ_PIN(PINB, QH))
            value |= 1;
        pulse_clk();
    }
    return value;
}

uint16_t get_led_data(uint8_t row, uint8_t row_data)
{
    uint16_t out_data = 0x001F;
    if (row == 0)
        out_data -= 0x01;
    if (row == 1)
        out_data -= 0x02;
    if (row == 2)
        out_data -= 0x04;
    if (row == 3)
        out_data -= 0x08;
    if (row == 4)
        out_data -= 0x10;

    if (row_data & (1 << 0))
        out_data += 0x20;
    if (row_data & (1 << 1))
        out_data += 0x40;
    if (row_data & (1 << 2))
        out_data += 0x80;
    if (row_data & (1 << 3))
        out_data += (1u << 8);
    if (row_data & (1 << 4))
        out_data += (2u << 8);

    return out_data;
}

// --- Gestione matrice 5x5 ---
uint8_t led_status[5] = {0, 0, 0, 0, 0};
uint8_t prev_buttons[6] = {0, 0, 0, 0, 0, 0}; // 6 righe adesso!
uint8_t shuffle_counter = 0;

static inline void toggle_led(uint8_t row, uint8_t col)
{
    if (row < 5 && col < 5)
    {
        led_status[row] ^= (1 << col);
    }
}

void handle_press(uint8_t row, uint8_t col)
{
    toggle_led(row, col);
    toggle_led(row - 1, col);
    toggle_led(row + 1, col);
    toggle_led(row, col - 1);
    toggle_led(row, col + 1);
}

// --- RNG semplice (LFSR 16-bit) ---
uint16_t lfsr = 0xACE1;
static inline uint16_t rand16()
{
    lfsr ^= lfsr << 7;
    lfsr ^= lfsr >> 9;
    lfsr ^= lfsr << 8;
    return lfsr;
}

// --- Shuffle N mosse random ---
void shuffle(uint8_t count)
{
    for (int i = 0; i < 5; i++)
        led_status[i] = 0;
    for (uint8_t i = 0; i < count; i++)
    {
        uint8_t r = rand16() % 5; // row 0..4
        uint8_t c = rand16() % 5; // col 0..4
        handle_press(r, c);
    }
}

int main(void)
{
    DDRB |= (1 << SER) | (1 << CLK) | (1 << RCLK) | (1 << SHLD);
    DDRB &= ~(1 << QH);
    SET_HIGH(PORTB, QH);

    SET_LOW(PORTB, CLK);
    SET_LOW(PORTB, RCLK);
    SET_HIGH(PORTB, SHLD);

    while (1)
    {
        for (uint8_t row = 0; row < 6; ++row)
        { // adesso 6 righe!
            uint16_t out_data = (uint16_t)0b1000000000000000 >> ((row < 5) ? row + 1 : 0);
            uint16_t led_data = (row < 5) ? get_led_data(row, led_status[4 - row]) : 0;
            out_data |= led_data;
            write_74hc595(out_data);

            uint8_t in_data = read_74hc165();
            uint8_t buttons = in_data & 0b11111;

            // Edge detection
            for (uint8_t col = 0; col < 5; ++col)
            {
                uint8_t mask = (1 << col);
                if ((buttons & mask) == 0 && (prev_buttons[row] & mask) != 0)
                {
                    if (row < 5)
                    {
                        handle_press(row, col);
                    }
                    else
                    {
                        // row == 5 → shuffle buttons
                        switch (col)
                        {
                        case 0:
                            shuffle(3);
                            break;
                        case 1:
                            shuffle(10);
                            break;
                        case 2:
                            shuffle(20);
                            break;
                        case 3:
                            shuffle(50);
                            break;
                        case 4:
                            shuffle(100);
                            break;
                        }
                    }
                }
            }
            prev_buttons[row] = buttons;
        }
        _delay_ms(1);
    }
    shuffle_counter += 17;
}