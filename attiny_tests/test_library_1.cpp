Codice c++

#include <TinyWireM.h> // Libreria per I2C su ATtiny85
#include <ADXL345.h>   // Libreria per il sensore accelerometro

// Definizione dei pin per i 74HC595
#define DATA_PIN 4  // SER - Data
#define CLOCK_PIN 1 // SRCLK - Clock
#define LATCH_PIN 3 // RCLK - Latch

    ADXL345 accelerometer; // Oggetto per gestire il sensore

void setup()
{
    TinyWireM.begin(); // Inizializza I²C

    // Configura i pin del 74HC595
    pinMode(DATA_PIN, OUTPUT);
    pinMode(CLOCK_PIN, OUTPUT);
    pinMode(LATCH_PIN, OUTPUT);
}

void loop()
{

    // Normalizzazione dei valori di accelerazione (tra 0 e 255)
    byte xLed = map(abs(x), 0, 256, 0, 255);
    byte yLed = map(abs(y), 0, 256, 0, 255);

    // Determina la disposizione dei LED in base all’accelerazione
    byte ledPattern1 = 0b00000000; // Primo registro 74HC595
    byte ledPattern2 = 0b00000000; // Secondo registro 74HC595

    if (x > 50)
        ledPattern1 = 0b00001111; // Esempio di stato LED
    if (x < -50)
        ledPattern1 = 0b11110000;
    if (y > 50)
        ledPattern2 = 0b00001111;
    if (y < -50)
        ledPattern2 = 0b11110000;

    // Invia i dati ai registri a scorrimento
    updateShiftRegister(ledPattern1, ledPattern2);

    delay(100);
}

// Funzione per aggiornare i 74HC595
void updateShiftRegister(byte firstByte, byte secondByte)
{
    digitalWrite(LATCH_PIN, LOW); // Disattiva latch

    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, secondByte); // Secondo registro
    shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, firstByte);  // Primo registro

    digitalWrite(LATCH_PIN, HIGH); // Attiva latch per aggiornare i LED
}
