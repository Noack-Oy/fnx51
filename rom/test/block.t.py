#!/usr/bin/env python3

from util import test_main

def run_test(pe):
    pe.logfile_read.raw = True;

    for i in range(0x20):
        pe.expect_exact(f"00000{i:02x}0:")
    pe.expect_exact("55 aa  |")
    pe.expect_exact("\r\n")

    for i in range(0x04):
        pe.expect_exact(f"00000{i:02x}0:")
    pe.expect_exact("\r\n")

    for i in range(0x20):
        pe.expect_exact(f"00000{i:02x}0:")
    pe.expect_exact("55 aa  |")
    pe.expect_exact("\r\n")

if __name__ == '__main__':
    test_main(run_test, "block.hex")
