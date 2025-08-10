import lgpio
import time

PIN_PL = 25
PIN_CP = 11
PIN_Q7 = 9

h = lgpio.gpiochip_open(0)

lgpio.gpio_claim_output(h, PIN_PL)
lgpio.gpio_claim_output(h, PIN_CP)
lgpio.gpio_claim_input(h, PIN_Q7)


def read_shift_register():
    bits = []

    # Latch basso -> alto per caricare
    lgpio.gpio_write(h, PIN_PL, 0)
    time.sleep(0.01)  # 2   s
    lgpio.gpio_write(h, PIN_PL, 1)
    time.sleep(0.01)

    # Leggi 8 bit MSB-first
    for _ in range(8):
        bits.append(lgpio.gpio_read(h, PIN_Q7))
        lgpio.gpio_write(h, PIN_CP, 1)
        time.sleep(0.01)
        lgpio.gpio_write(h, PIN_CP, 0)
        time.sleep(0.01)

    return bits


try:
    while True:
        stato = read_shift_register()
        print(stato)
        time.sleep(0.2)

except KeyboardInterrupt:
    pass

finally:
    lgpio.gpiochip_close(h)
