#include <avr/io.h>
#include <util/delay.h>

#define SHLD PB4 // Latch pin (SH/LD)
#define CLK PB2  // Clock pin
#define QH PB0   // Data input from 74HC165

// Funzione per leggere un byte dal 74HC165
uint8_t shiftIn74HC165()
{
    uint8_t value = 0;

    // Porta SH/LD basso per caricare i dati paralleli
    PORTB &= ~(1 << SHLD);
    _delay_us(5);

    // Porta SH/LD alto per abilitare lo shift
    PORTB |= (1 << SHLD);
    _delay_us(5);

    // Leggi 8 bit
    for (uint8_t i = 0; i < 8; i++)
    {
        // Porta CLK basso
        PORTB &= ~(1 << CLK);
        _delay_us(1);

        // Leggi il bit presente su QH
        value <<= 1;
        if (PINB & (1 << QH))
        {
            value |= 1;
        }

        // Porta CLK alto per spostare al bit successivo
        PORTB |= (1 << CLK);
        _delay_us(1);
    }

    return value;
}

int main()
{
    // Configurazione pin
    DDRB |= (1 << SHLD) | (1 << CLK); // SH/LD e CLK come output
    DDRB &= ~(1 << QH);               // QH come input

    // Imposta default: SH/LD alto, CLK basso
    PORTB |= (1 << SHLD);
    PORTB &= ~(1 << CLK);

    // Configura pull-up su QH (opzionale, in caso di floating)
    PORTB |= (1 << QH);

    // LED di test su PB1 (opzionale)
    DDRB |= (1 << PB1);

    while (1)
    {
        uint8_t data = shiftIn74HC165();

        // Come test: accendi LED se il primo bit è alto
        if (data & 0x80)
        {
            PORTB |= (1 << PB1);
        }
        else
        {
            PORTB &= ~(1 << PB1);
        }

        _delay_ms(200);
    }
}