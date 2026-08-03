#!/usr/bin/env python3

import sys
sys.path.append('..')

from loader import *

def run_test(pe):
        pe.logfile_read.raw=True
        pe.send("D0;10;")
        pe.expect("00000000: 04 00 fc 06  00 00 fc 06")
        pe.expect("\r\n")

        # allocate first block
        pe.send("A0123;")
        pe.expect("0004\r\n")
        pe.send("D0000;10;")
        pe.expect("00000000: 28 01 d8 05")
        pe.expect("\r\n")
        pe.send("D0128;10;")
        pe.expect("00000128: 00 00 d8 05")
        pe.expect("\r\n")

        # allocate another block
        pe.send("A0312;")
        pe.expect("0128\r\n")
        pe.send("D0000;10;")
        pe.expect("00000000: 3c 04 c4 02")
        pe.expect("\r\n")
        pe.send("D043c;10;")
        pe.expect("0000043c: 00 00 c4 02")
        pe.expect("\r\n")

        # allocate third block
        pe.send("A01c4;")
        pe.expect("043c\r\n")
        pe.send("D0000;10;")
        pe.expect("00000000: 00 06 00 01")
        pe.expect("\r\n")

        # release second block
        pe.send("R0128;0312;")
        pe.expect("\r\n")
        pe.send("D0000;10;")
        pe.expect("00000000: 28 01 14 04")
        pe.expect("\r\n")
        pe.send("D0128;10;")
        pe.expect("00000128: 00 06 14 03")
        pe.expect("\r\n")

        # release third block
        pe.send("R043c;01c4;")
        pe.expect("\r\n")
        pe.send("D0128;10;")
        pe.expect("00000128: 00 00 d8 05")
        pe.expect("\r\n")

        # release part of first block
        pe.send("R0004;0100;")
        pe.expect("\r\n")
        pe.send("D0000;10;")
        pe.expect("00000000: 04 00 d8 06  28 01 00 01")
        pe.expect("\r\n")

        # release everything
        pe.send("R0104;0023;")
        pe.expect("\r\n")
        pe.send("D0000;10;")
        pe.expect("00000000: 04 00 fc 06  00 00 fc 06")
        pe.expect("\r\n")

        # exercise the "skip too-small block" path in memory_allocate by
        # setting up two free blocks (a small one at the head, a big one
        # further along), then requesting more than the small one can hold.

        # allocate two 256-byte chunks, back-to-back
        pe.send("A0100;")
        pe.expect("0004\r\n")
        pe.send("A0100;")
        pe.expect("0104\r\n")

        # release the first chunk: leaves a 256-byte hole at 0x0004 and a
        # large free remainder starting at 0x0204
        pe.send("R0004;0100;")
        pe.expect("\r\n")
        pe.send("D0000;10;")
        # head: first=0x0004, free=0x05fc
        # block at 0x0004: next=0x0204, size=0x0100
        pe.expect("00000000: 04 00 fc 05  04 02 00 01")
        pe.expect("\r\n")

        # ask for more than fits in the first free block: the allocator
        # must walk past it to reach 0x0204
        pe.send("A0200;")
        pe.expect("0204\r\n")
        pe.send("D0000;10;")
        # head: first=0x0004, free=0x03fc
        # block at 0x0004: next=0x0404 (advanced past the split), size=0x0100
        pe.expect("00000000: 04 00 fc 03  04 04 00 01")
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
