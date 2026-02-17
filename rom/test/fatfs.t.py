#!/usr/bin/env python3

from util import test_main

def run_test(pe):
    pe.logfile_read.raw = True;

    # Expect fatfs_info structure dump
    pe.expect_exact(f"00000000: 80 00 00 00  08 aa 04 00  00 95 22 00  00 70 40 00")
    pe.expect_exact(f"00000010: 00 02 00 00  00")
    pe.expect_exact("\r\n")
    
    # Expect root cluster number
    pe.expect_exact("00000002")
    pe.expect_exact("\r\n")
    
    # Expect cluster-to-LBA conversion result (cluster 2 with cluster_lba 0x00004070)
    pe.expect_exact("00004080")
    pe.expect_exact("\r\n")

if __name__ == '__main__':
    test_main(run_test, "fatfs.hex")
