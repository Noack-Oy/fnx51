#!/usr/bin/env python3

import sys
sys.path.append('..')

from loader import *


def run_test(pe):
    pe.logfile_read.raw = True;
    pe.expect_exact("00000080: 48 65 6c 6c  6f 20 66 72  6f 6d 20 69  6e 74 65 72  |Hello from inter|\r\n")
    pe.expect_exact("00000090: 6e 61 6c 20  6d 65 6d 6f  72 79 3a 20  31 35 36 ff  |nal memory: 156.|\r\n")
    pe.expect_exact("000000f0:")
    pe.expect_exact("\r\n")

if __name__ == '__main__':
    try:
        pe = open_serial()
        enter_bootloader()
        write_hex_file('stream.hex')
        reset()
        run_test(pe)
    finally:
        close_serial()
