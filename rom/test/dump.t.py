#!/usr/bin/env python3

import sys
sys.path.append('..')

from loader import *


def run_test(pe):
    pe.logfile_read.raw = True;
    pe.send(b"Hello, world!\r\n\xff")
    pe.expect_exact("00000000: 48 65 6c 6c  6f 2c 20 77  6f 72 6c 64  21 0d 0a ff  |Hello, world!...|\r\n")
    pe.send(b"This is a test.\x7f")
    pe.expect_exact("00000010: 54 68 69 73  20 69 73 20  61 20 74 65  73 74 2e 7f  |This is a test..|\r\n")
    pe.send(b"\x00End.")
    pe.expect_exact("00000020: 00 45 6e 64  2e                                     |.End.|\r\n")
    pe.expect_exact("00000025\r\n")

if __name__ == '__main__':
    try:
        pe = open_serial()
        enter_bootloader()
        write_hex_file('dump.hex')
        reset()
        run_test(pe)
    finally:
        close_serial()
