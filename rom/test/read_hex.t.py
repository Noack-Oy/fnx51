#!/usr/bin/env python3

import sys
sys.path.append('..')

from loader import *

def run_test(pe):

        for t in ['\t', ' ', '/', ':', '@', 'g', '~']:
            pe.send('1'+t)
            pe.expect('1')

        for n in range(1,9):
            x = "1234abcd"[:n]
            pe.send(x+"!")
            time.sleep(0.1)
            pe.expect(x)

        pe.send("1234abcde")
        time.sleep(0.1)
        pe.expect("234abcde")

        pe.send("00000000000000004321!")
        time.sleep(0.1)
        pe.expect("4321")

        pe.send("cCdDeEfF!")
        time.sleep(0.1)
        pe.expect("ccddeeff")

if __name__ == '__main__':
    try:
        pe = open_serial()
        enter_bootloader()
        write_hex_file('read_hex.hex')
        reset()
        run_test(pe)
    finally:
        close_serial()
