#!/usr/bin/env python3

import sys
sys.path.append('..')

from loader import *


def run_test(pe):
        pe.logfile_read.raw = True;
        pe.expect('Hello, world!\r\n')

        for i in range(0, 0x100):
           pe.expect(f'{i:02x}')
        pe.expect('\r\n')

        for i in range(0, 0x10000, 0x55):
           pe.expect(f'{i:04x}')
        pe.expect('\r\n')

        m = 2**32
        a = 0
        b = 1
        while a < m:
            pe.expect(f'{a:08x}')
            c = a + b
            a = b
            b = c
        pe.expect('\r\n')

        for i in range(0, 256):
            pe.expect(str(i))
        pe.expect('\r\n')

        for i in range(-128, 128):
            pe.expect(str(i))
        pe.expect('\r\n')

        for i in range(0, 65536, 51):
            pe.expect(str(i))
        pe.expect('\r\n')

        for i in range(-32768, 32768, 127):
            pe.expect(str(i))
        pe.expect('\r\n')

if __name__ == '__main__':
    try:
        pe = open_serial()
        enter_bootloader()
        write_hex_file('print.hex')
        reset()
        run_test(pe)
    finally:
        close_serial()
