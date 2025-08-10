# this script works when driving the registers with the raspberry. No misalignments
import lgpio
import time

h = lgpio.gpiochip_open(0)

PIN_SER = 11
PIN_SRCLK = 9
PIN_RCLK = 10

# Setta i pin come output
for pin in [PIN_SER, PIN_SRCLK, PIN_RCLK]:
    lgpio.gpio_claim_output(h, pin, 0)


def pulse(pin):
    lgpio.gpio_write(h, pin, 1)
    time.sleep(0.00001)
    lgpio.gpio_write(h, pin, 0)
    time.sleep(0.00001)


def shift_out(byte1, byte2):
    for byte in (byte1, byte2):
        for i in range(7, -1, -1):
            lgpio.gpio_write(h, PIN_SER, (byte >> i) & 1)
            pulse(PIN_SRCLK)
    pulse(PIN_RCLK)


def put_pixel(row, col):
    byte1 = 0xFF
    byte2 = 0xE0
    if row == 0:
        byte2 -= 0x20
    elif row == 1:
        byte2 -= 0x40
    elif row == 2:
        byte2 -= 0x80
    elif row == 3:
        byte1 -= 0x40
    else:
        byte1 -= 0x80
    if col == 0:
        byte2 += 0x10
    elif col == 1:
        byte2 += 8
    elif col == 2:
        byte2 += 4
    elif col == 3:
        byte2 += 2
    else:
        byte2 += 1
    print(bin(byte1) + " - " + bin(byte2))
    print(str(row) + " - " + str(col))
    shift_out(byte1, byte2)


try:
    print("Inserisci 16 bit (es. 1010101010101010) oppure 'q' per uscire.")
    while True:
        # data = input("> ").strip()
        # if data.lower() == "q":
        #    break
        # if len(data) != 16 or not all(c in '01' for c in data):
        #    print("Input non valido. Inserisci esattamente 16 bit.")
        #    continue
        # bits = int(data, 2)
        # shift_out((bits >> 8) & 0xFF, bits & 0xFF)
        for ix in range(5):
            for iy in range(5):
                put_pixel(ix, iy)
                time.sleep(0.02)
except KeyboardInterrupt:
    pass
finally:
    lgpio.gpiochip_close(h)
