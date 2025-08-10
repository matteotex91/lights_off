#define F_CPU 1000000UL // Clock a 1MHz

#include <avr/io.h>
#include <util/delay.h>

#define SER PB1   // Pin 6
#define SRCLK PB2 // Pin 7
#define RCLK PB3  // Pin 2

void pulse(uint8_t pin)
{
    PORTB |= (1 << pin);
    _delay_us(1);
    PORTB &= ~(1 << pin);
    _delay_us(1);
}

// void shiftOut(uint8_t data)
// {
//     for (int8_t i = 7; i >= 0; i--)
//     {
//         if (data & (1 << i))
//             PORTB |= (1 << SER);
//         else
//             PORTB &= ~(1 << SER);
//         pulse(SRCLK);
//     }
// }

void shiftOut(uint8_t data)
{
    for (int8_t i = 7; i >= 0; i--)
    {
        // Set SER (dati) PRIMA di far salire SRCLK
        if (data & (1 << i))
            PORTB |= (1 << SER);
        else
            PORTB &= ~(1 << SER);

        // Piccola attesa per assicurare stabilità
        _delay_us(5);

        // Fronte di salita di SRCLK per shiftare il bit
        PORTB |= (1 << SRCLK);
        _delay_us(5);

        // Fronte di discesa di SRCLK
        PORTB &= ~(1 << SRCLK);
        _delay_us(5);
    }
}

void latch()
{
    // Fronte di salita su RCLK per aggiornare l'uscita
    PORTB |= (1 << RCLK);
    _delay_us(5);
    PORTB &= ~(1 << RCLK);
    _delay_us(5);
}

int main()
{
    // Imposta pin come output
    DDRB |= (1 << SER) | (1 << SRCLK) | (1 << RCLK);

    // Sequenze di 8 bit da inviare
    const uint8_t patterns[] = {
        0,
        0,
        1,
        0,
        0,
        1,
        1,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        1,
        1,
    };

    while (1)
    {
        for (uint8_t shift = 0; shift < 16; shift++)
        {
            for (uint8_t i = 0; i < sizeof(patterns); i++)
            {
                shiftOut(patterns[(i + shift) % 16]);
            }
            latch();
            _delay_ms(500);
        }
    }

    return 0;
}