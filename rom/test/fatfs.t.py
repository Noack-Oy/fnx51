#!/usr/bin/env python3

from util import test_main

def run_test(pe):
    pe.logfile_read.raw = True;

    pe.expect_exact(f"00000000:")
    pe.expect_exact(f"00000010:")
    pe.expect_exact("\r\n")

if __name__ == '__main__':
    test_main(run_test, "fatfs.hex")
