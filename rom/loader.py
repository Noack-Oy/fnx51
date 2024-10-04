#!/usr/bin/env python3

import serial, time, sys
import pexpect.fdpexpect

ser = None
pe = None


def open_serial():
    global ser, pe
    ser = serial.Serial()
    ser.dtr = False
    ser.rts = False
    ser.port = '/dev/ttyUSB0'
    ser.bytesize = serial.EIGHTBITS
    ser.parity = serial.PARITY_NONE
    ser.open()
    pe = pexpect.fdpexpect.fdspawn(ser)
    pe.timeout = 1
    return pe

def close_serial():
    ser.close()


class Logger:
    def __init__(self, prefix):
        self.prefix = prefix

    def write(self, data):
        if isinstance(data, bytes):
            data = data.decode('utf-8')
        sys.stdout.write(self.prefix + repr(data)[1:-1] + '\n')

    def flush(self):
        sys.stdout.flush()


def cmd(function, data):
    length = "{:02X}".format(len(data)//2)
    line = length+'0000'+function+data
    bytes = [int(line[i:i+2], 16) for i in range(0, len(line), 2)]
    total = sum(bytes)%256
    check = (256-total)%256
    check = "{:02X}".format(check)
    line = ':' + line + check
    pe.send(line)
    ser.flush()
    time.sleep(0.1)
    pe.expect(line)
    pe.expect('.\r\n')


def enter_bootloader():
    print('BOOTLOADER')
    ser.dtr = False
    ser.rts = False
    time.sleep(0.1)
    ser.dtr = True
    time.sleep(0.2)
    ser.rts = True

    pe.logfile_send = Logger('> ')
    pe.logfile_read = Logger('< ')

    pe.send('U')
    pe.expect('U')

    cmd('05', '0B00') # read hardware security byte
    hsb = int(pe.before, 16)
    if not (hsb & 0x40): # check Boot Loader Jump Bit
        cmd('03', '0A0401') # set BLJB
    if (hsb & 0x80): # check X2 mode
        cmd('03', '0A0800') # enable X2 mode


def write_hex_file(filename):
    print(f'WRITE: {filename}')
    # write code
    with open(filename, 'r') as hexfile:
        for line in hexfile:
            pe.send(line.strip())
            ser.flush()
            time.sleep(0.1)
            pe.expect(line.strip()+'.\r\n')


def reset():
    print('RESET')
    ser.dtr = False
    time.sleep(0.1)
    ser.dtr = True
    time.sleep(0.1)


if __name__ == '__main__':
    try:
        open_serial()
        enter_bootloader()
        write_hex_file(sys.argv[1])
        reset()

    finally:
        close_serial()
