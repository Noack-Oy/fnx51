#!/usr/bin/env python3

import sys
sys.path.append('..')

from loader import *

def run_test(pe):
        pe.logfile_read.raw=True
        pe.send("d0;10;")
        pe.expect("00000000: 04 00 fc 06  00 00 fc 06")
        pe.expect("\r\n")

        pe.send("a123;")
        pe.expect("00000004\r\n")
        pe.send("d0;10;")
        pe.expect("\r\n")
        pe.send("d128;10;")
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
