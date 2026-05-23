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

    # Chain handle exercise: dump the first 32 bytes of the root directory.
    # On the reference card the first entry is the volume label "FNX51"
    # (attribute byte 0x08) followed by zeroed reserved/creation fields and
    # the write-time/date bytes captured when the volume was formatted.
    pe.expect_exact("00000000: 46 4e 58 35  31 20 20 20  20 20 20 08  00 00 00 00")
    pe.expect_exact("\r\n")
    pe.expect_exact("00000010: 00 00 00 00  00 00 53 aa  5d 59 00 00  00 00 00 00")
    pe.expect_exact("\r\n")

    # Dir iterator walk over the root directory on the reference card.
    # Three short-name entries: the volume label, a Windows system
    # directory, and an archived text file. LFN entries (if any) are
    # silently skipped by dir_next.
    pe.expect_exact("00000000: 46 4e 58 35  31 20 20 20  20 20 20 08  00 00 00 00")
    pe.expect_exact("\r\n")
    pe.expect_exact("00000010: 00 00 00 00  00 00 53 aa  5d 59 00 00  00 00 00 00")
    pe.expect_exact("\r\n")

    pe.expect_exact("00000000: 53 59 53 54  45 4d 7e 31  20 20 20 16  00 4b 52 aa")
    pe.expect_exact("\r\n")
    pe.expect_exact("00000010: 5d 59 5d 59  00 00 53 aa  5d 59 03 00  00 00 00 00")
    pe.expect_exact("\r\n")

    pe.expect_exact("00000000: 41 4c 49 43  45 27 7e 31  54 58 54 20  00 a3 0e ac")
    pe.expect_exact("\r\n")
    pe.expect_exact("00000010: 5d 59 5d 59  00 00 0f ac  5d 59 06 00  5e 44 02 00")
    pe.expect_exact("\r\n")

    # File open + read: dump the first 32 bytes of ALICE'~1TXT.
    pe.expect_exact("00000000: 41 6c 69 63  65 27 73 20  41 64 76 65  6e 74 75 72")
    pe.expect_exact("\r\n")
    pe.expect_exact("00000010: 65 73 20 69  6e 20 57 6f  6e 64 65 72  6c 61 6e 64")
    pe.expect_exact("\r\n")

if __name__ == '__main__':
    test_main(run_test, "fatfs.hex")
