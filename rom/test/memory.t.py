#!/usr/bin/env python3

import sys
sys.path.append('..')

from loader import *

def run_test(pe):
        pe.logfile_read.raw=True
        pe.send("d0;10;")
        pe.expect("00000000: 04 00 fc 06  00 00 fc 06")
        pe.expect("\r\n")

        # allocate first block
        pe.send("a123;")
        pe.expect("00000004\r\n")
        pe.send("d0;10;")
        pe.expect("00000000: 28 01 d8 05")
        pe.expect("\r\n")
        pe.send("d128;10;")
        pe.expect("00000128: 00 00 d8 05")
        pe.expect("\r\n")

        # allocate another block
        pe.send("a312;")
        pe.expect("00000128\r\n")
        pe.send("d0;10;")
        pe.expect("00000000: 3c 04 c4 02")
        pe.expect("\r\n")
        pe.send("d43c;10;")
        pe.expect("0000043c: 00 00 c4 02")
        pe.expect("\r\n")

        # allocate third block
        pe.send("a1c4;")
        pe.expect("0000043c\r\n")
        pe.send("d0;10;")
        pe.expect("00000000: 00 06 00 01")
        pe.expect("\r\n")

        # release second block
        pe.send("r128;312;")
        pe.send("d0;10;")
        pe.expect("00000000: 28 01 12 04")
        pe.expect("\r\n")
        pe.send("d128;10;")
        pe.expect("00000128: 00 06 12 03")
        pe.expect("\r\n")


if __name__ == '__main__':
    try:
        pe = open_serial()
        enter_bootloader()
        write_hex_file('memory.hex')
        reset()
        run_test(pe)
    finally:
        close_serial()
