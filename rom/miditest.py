#!/usr/bin/env python3

import mido
from loader import *
import time

# Load firmware and reset microcontroller
pe = open_serial()
enter_bootloader()
write_hex_file('rom.hex')
reset()

# Open MIDI input
midi_input_name = 'Oxygen 49:Oxygen 49 MIDI 1 24:0'

# Define default tuning points
default_note_table = {
    36: 0xff4f,  # C2
    48: 0xfc77,  # C3
    60: 0xf988,  # C4
    72: 0xf67d,  # C5
    84: 0xf338   # C6
}

# Copy default values to allow adjustments
note_table = default_note_table.copy()

# Slider to note index mapping based on control messages
slider_to_note = {
    73: 36,  # C2
    72: 48,  # C3
    5:  60,  # C4
    84: 72,  # C5
    7:  84   # C6
}

# Generate linear interpolation table
def generate_pwm_table():
    global note_to_pwm
    note_to_pwm = {}
    note_keys = sorted(note_table.keys())

    for i in range(note_keys[0], note_keys[-1] + 1):
        for j in range(len(note_keys) - 1):
            if note_keys[j] <= i <= note_keys[j + 1]:
                pwm_value = int(note_table[note_keys[j]] +
                                (note_table[note_keys[j + 1]] - note_table[note_keys[j]]) *
                                (i - note_keys[j]) / (note_keys[j + 1] - note_keys[j]))
                note_to_pwm[i] = f'{pwm_value:04x};'
                break

# Initialize PWM table
generate_pwm_table()

# Open the MIDI port
with mido.open_input(midi_input_name) as port:
    print("Listening for MIDI messages...")

    for msg in port:
        if msg.type == 'note_on' and msg.velocity > 0:  # Note-on event
            note = msg.note
            if note in note_to_pwm:
                hex_value = note_to_pwm[note]
                print(f"Note {note} -> {hex_value}")
                pe.send(hex_value)
        elif msg.type == 'control_change':  # Adjust tuning points
            slider = msg.control
            value = msg.value

            if slider in slider_to_note:
                index = slider_to_note[slider]  # Map to note index
                adjustment = 64 - value  # Invert direction (-64 to +64)
                note_table[index] = default_note_table[index] + adjustment
                print(f"Adjusted tuning point {index} to {note_table[index]:04x}")
                generate_pwm_table()  # Rebuild the table

                # Play adjusted note immediately
                hex_value = note_to_pwm[index]
                pe.send(hex_value)
            else:
                print(f"Unexpected CC message: Control {slider}, Value {value}")

# Cleanup
pe.close()
