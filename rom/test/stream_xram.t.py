#!/usr/bin/env python3

import sys
sys.path.append('..')

from loader import *


def run_test(pe):
    pe.logfile_read.raw = True;
    pe.expect_exact("00000080:")
    pe.expect_exact("00000100:")
    pe.expect_exact("00000180: 48 65 6c 6c  6f 2c 20 58  52 41 4d 3a  20 33 39 37  |Hello, XRAM: 397|\r\n")
    pe.expect_exact("000001f0:")
    pe.expect_exact("\r\n")

if __name__ == '__main__':
    try:
        pe = open_serial()
        enter_bootloader()
        write_hex_file('stream_xram.hex')
        reset()
        run_test(pe)
    finally:
        close_serial()
