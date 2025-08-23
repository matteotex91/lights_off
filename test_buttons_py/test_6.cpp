#define F_CPU 8000000UL // <-- cambia a 1000000UL se usi CKDIV8 (1 MHz)
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

// --- Pin mapping su PORTB (ATtiny85) ---
#define QH PB0   // input dal 74HC165 (QH, pin 9 sul 165)
#define SER PB1  // data verso 74HC595 (SER, pin 14 sul 595)
#define CLK PB2  // clock condiviso (SRCLK 595, CP 165)
#define RCLK PB3 // latch 74HC595 (RCK, pin 12)
#define SHLD PB4 // SH/LD 74HC165 (pin 1)

// Helper macro
#define SET_HIGH(port, bit) ((port) |= (1 << (bit)))
#define SET_LOW(port, bit) ((port) &= ~(1 << (bit)))
#define READ_PIN(pin, bit) ((pin) & (1 << (bit)))

// Un piccolo impulso di clock comune (fronte di salita valido per 165/595)
static inline void pulse_clk(void)
{
    SET_HIGH(PORTB, CLK);
    _delay_us(2);
    SET_LOW(PORTB, CLK);
    _delay_us(2);
}

// Scrive 16 bit nei due 74HC595 in cascata (MSB first)
void write_74hc595(uint16_t value, uint8_t bits = 16)
{
    for (int8_t i = bits - 1; i >= 0; --i)
    {
        if (value & (1u << i))
            SET_HIGH(PORTB, SER);
        else
            SET_LOW(PORTB, SER);
        pulse_clk(); // il 595 campiona SER sul fronte di salita
    }
    // Latch per aggiornare le uscite del 595 (i clock successivi non cambiano le uscite)
    SET_HIGH(PORTB, RCLK);
    _delay_us(2);
    SET_LOW(PORTB, RCLK);
}

// Legge 8 bit dal 74HC165 (MSB first su QH)
// Nota: i clock emessi qui non alterano le uscite del 595 perché RCLK resta fermo
uint8_t read_74hc165(void)
{
    uint8_t value = 0;

    // SH/LD basso: carica i livelli paralleli nei latch interni
    SET_LOW(PORTB, SHLD);
    _delay_us(5);
    // SH/LD alto: abilita lo shift seriale su QH
    SET_HIGH(PORTB, SHLD);
    _delay_us(5);

    for (uint8_t i = 0; i < 8; ++i)
    {
        value <<= 1;
        if (READ_PIN(PINB, QH))
            value |= 1; // leggi il bit attuale su QH
        pulse_clk();    // fronte per avanzare al bit successivo
    }
    return value;
}

// Stessa logica della tua get_led_data Python
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

int main(void)
{
    // --- Configurazione direzione pin ---
    // Output: SER, CLK, RCLK, SHLD
    DDRB |= (1 << SER) | (1 << CLK) | (1 << RCLK) | (1 << SHLD);
    // Input: QH
    DDRB &= ~(1 << QH);

    // Pull-up su QH (in caso di linea flottante)
    SET_HIGH(PORTB, QH);

    // Stati iniziali sicuri
    SET_LOW(PORTB, CLK);
    SET_LOW(PORTB, RCLK);
    SET_HIGH(PORTB, SHLD); // modalità shift disabilitata finché non carichiamo

    uint8_t led_status[5] = {0, 0, 0, 0, 0};

    for (;;)
    {
        for (uint8_t i = 0; i < 5; ++i)
        {
            // Pattern come nello script: bit di “riga” nei 16 bit dei 595
            uint16_t out_data = (uint16_t)0b1000000000000000 >> (i + 1);
            uint16_t led_data = get_led_data(i, led_status[4 - i]);
            out_data |= led_data;

            write_74hc595(out_data);

            uint8_t in_data = read_74hc165();
            led_status[i] = in_data & 0b11111; // usa solo i primi 5 bit
        }
        _delay_ms(1);
    }
}