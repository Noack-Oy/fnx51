#!/usr/bin/env python3

import sys
sys.path.append('..')

from loader import *

def run_test(pe):
        pe.expect('!!! panic @001f')

if __name__ == '__main__':
    try:
        pe = open_serial()
        enter_bootloader()
        write_hex_file('panic.hex')
        reset()
        run_test(pe)
    finally:
        close_serial()
