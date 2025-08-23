import lgpio
import time

# GPIO mapping (BCM numbering)
PIN_QH = 10  # PB0
PIN_SHLD = 9  # PB4
PIN_CLK = 11  # PB2

PIN_SER = 13  # PB1
PIN_RCLK = 25  # PB3

# Apri handle sul chip gpio0
h = lgpio.gpiochip_open(0)

# Configura pin
lgpio.gpio_claim_output(h, PIN_SHLD)
lgpio.gpio_claim_output(h, PIN_CLK)
lgpio.gpio_claim_output(h, PIN_SER)
lgpio.gpio_claim_output(h, PIN_RCLK)

lgpio.gpio_claim_input(h, PIN_QH)


# Utility: scrive 16 bit nei due 74HC595 (MSB first)
def write_74hc595(value, bits=16):
    for i in range(bits - 1, -1, -1):
        bit = (value >> i) & 1
        lgpio.gpio_write(h, PIN_SER, bit)
        # Clock
        lgpio.gpio_write(h, PIN_CLK, 1)
        time.sleep(0.000001)
        lgpio.gpio_write(h, PIN_CLK, 0)
        time.sleep(0.000001)
    # Latch
    lgpio.gpio_write(h, PIN_RCLK, 1)
    time.sleep(0.000001)
    lgpio.gpio_write(h, PIN_RCLK, 0)


# Utility: legge 8 bit dal 74HC165
def read_74hc165():
    value = 0
    # SH/LD basso per caricare ingressi paralleli
    lgpio.gpio_write(h, PIN_SHLD, 0)
    time.sleep(0.000001)
    lgpio.gpio_write(h, PIN_SHLD, 1)
    time.sleep(0.000001)

    for i in range(8):
        value <<= 1
        if lgpio.gpio_read(h, PIN_QH):
            value |= 1
        # Clock
        lgpio.gpio_write(h, PIN_CLK, 1)
        time.sleep(0.000001)
        lgpio.gpio_write(h, PIN_CLK, 0)
        time.sleep(0.000001)

    return value


def get_led_data(row, row_data):
    out_data = 0x1F
    if row == 0:
        out_data -= 0x01
    if row == 1:
        out_data -= 0x02
    if row == 2:
        out_data -= 0x04
    if row == 3:
        out_data -= 0x08
    if row == 4:
        out_data -= 0x10
    if row_data & (1 << 0):
        out_data += 0x20
    if row_data & (1 << 1):
        out_data += 0x40
    if row_data & (1 << 2):
        out_data += 0x80
    if row_data & (1 << 3):
        out_data += 1 << 8
    if row_data & (1 << 4):
        out_data += 2 << 8
    return out_data


try:
    led_status = [0, 0, 0, 0, 0]
    while True:
        for i in range(5):
            out_data = 0b1000000000000000 >> (i + 1)
            led_data = get_led_data(i, led_status[4 - i])
            out_data |= led_data
            write_74hc595(out_data)
            in_data = read_74hc165()
            # if i < 5:
            led_status[i] = in_data & 0b11111
            # print(format(in_data & 0b11111, "08b"))

        # Usa i primi 5 bit (da MSB a LSB)

        # for i in range(8):
        #     state = (data >> (7 - i)) & 1
        #     print(f"Bottone {i + 1}: {'Premuto' if state == 0 else 'Rilasciato'}")
        # print("------")
        time.sleep(0.001)

except KeyboardInterrupt:
    print("Uscita dal programma")

finally:
    lgpio.gpiochip_close(h)
