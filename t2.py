import RPi.GPIO as GPIO
import time

# Definizione dei pin
PIN_PL = 25  # Latch (Parallel Load)
PIN_CP = 11  # Clock
PIN_Q7 = 9  # Data Output

# Impostazione della modalit   GPIO
GPIO.setmode(GPIO.BCM)
GPIO.setup(PIN_PL, GPIO.OUT)
GPIO.setup(PIN_CP, GPIO.OUT)
GPIO.setup(PIN_Q7, GPIO.IN)


def read_shift_register():
    """Legge lo stato dei pulsanti dal 74HC165"""
    GPIO.output(PIN_PL, GPIO.LOW)  # Carica i dati nel registro
    time.sleep(0.01)
    GPIO.output(PIN_PL, GPIO.HIGH)  # Latch i dati

    button_states = 0
    state = ""

    for i in range(8):  # Il 74HC165 ha 8 bit di output
        button_states |= GPIO.input(PIN_Q7) << (7 - i)  # Legge i bit
        state = state + str(GPIO.input(PIN_Q7))
        GPIO.output(PIN_CP, GPIO.HIGH)  # Impulso di clock
        time.sleep(0.001)
        GPIO.output(PIN_CP, GPIO.LOW)
    print(state)
    return button_states


try:
    while True:
        button_states = read_shift_register()
        for i in range(8):  # Solo i primi 4 pulsanti
            state = (button_states >> (7 - i)) & 1
            print(f"Pulsante {i + 1}: {'Premuto' if state == 0 else 'Rilasciato'}")
        time.sleep(0.5)

except KeyboardInterrupt:
    GPIO.cleanup()
