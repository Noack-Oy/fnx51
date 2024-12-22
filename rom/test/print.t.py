#!/usr/bin/env python3

import sys
sys.path.append('..')

from loader import *


def run_test(pe):
        pe.logfile_read.raw = True;

        # text: "Hello, World!"
        pe.expect('Hello, world!\r\n')

        # hex_8: hex numbers 00..ff
        for i in range(0, 0x100):
           pe.expect(f'{i:02x}')
        pe.expect('\r\n')

        # hex_16: hex numbers 0000..ffff in steps of 55 (hex)
        for i in range(0, 0x10000, 0x55):
           pe.expect(f'{i:04x}')
        pe.expect('\r\n')

        # hex_32: fibonacci sequence (in hex)
        m = 2**32
        a = 0
        b = 1
        while a < m:
            pe.expect(f'{a:08x}')
            c = a + b
            a = b
            b = c
        pe.expect('\r\n')

        # int_u8: all integers from 0 to 255
        for i in range(0, 256):
            pe.expect(str(i))
        pe.expect('\r\n')

        # int_s8: all integers from -128 to 127
        for i in range(-128, 128):
            pe.expect(str(i))
        pe.expect('\r\n')

        # int_u16: integers from 0 to 65535 in steps of 51 (decimal)
        for i in range(0, 65536, 51):
            pe.expect(str(i))
        pe.expect('\r\n')

        # int_s16: start at -32768 and add 127 until signed overflow
        for i in range(-32768, 32768, 127):
            pe.expect(str(i))
        pe.expect('\r\n')

        # int_u32: fibonacci sequence (in decimal)
        m = 2**32
        a = 0
        b = 1
        while a < m:
            pe.expect(str(a))
            c = a + b
            a = b
            b = c
        pe.expect('\r\n')

        # int_s32: start at -2147483648 and add 123456789 repeatedly
        for i in range(-2147483648, 2147483647, 123456789):
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
