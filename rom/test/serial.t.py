#!/usr/bin/env python3

from util import test_main

def run_test(pe):
        pe.expect('Hello, world!\r\n')
        pe.send("Test")
        pe.expect("Uftu")

if __name__ == '__main__':
    test_main(run_test, "serial.hex")
