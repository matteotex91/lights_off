import lgpio
import time

# GPIO mapping (BCM numbering)
PIN_QH = 10  # 74HC165 QH (output data)
PIN_SHLD = 9  # 74HC165 SH/LD
PIN_CLK = 11  # clock comune 74HC165 / 74HC595

PIN_SER = 13  # 74HC595 SER (data in)
PIN_RCLK = 25  # 74HC595 latch (RCK)

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
    out_data = 0b11111
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
        out_data += 0x100
    if row_data & (1 << 4):
        out_data += 0x200
    return out_data


try:
    while True:
        for col in range(5):
            for row in range(5):
                write_74hc595(get_led_data(col, 1 << row))
                time.sleep(0.5)
            # print(format(in_data & 0b11111, "08b"))

        print("--------")
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
