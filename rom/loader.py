#!/usr/bin/env python3

import serial, time, sys
import pexpect.fdpexpect

device = '/dev/ttyUSB0'
ser = None
pe = None


def open_serial():
    global ser, pe
    ser = serial.Serial()
    ser.dtr = False
    ser.rts = False
    ser.port = device
    ser.bytesize = serial.EIGHTBITS
    ser.parity = serial.PARITY_NONE
    ser.open()
    time.sleep(0.5)
    while ser.in_waiting:
        ser.reset_input_buffer()
        time.sleep(0.1)
    pe = pexpect.fdpexpect.fdspawn(ser, encoding=None)
    pe.timeout = 1
    pe.logfile_send = Logger('> ')
    pe.logfile_read = Logger('< ')
    return pe

def close_serial():
    ser.close()

def flush_serial():
    ser.flush()

def flush():
    ser.flush()
    while ser.in_waiting:
        ser.reset_input_buffer()
        time.sleep(0.1)
    try:
        while True:
            pe.read_nonblocking(size=1024, timeout=0.1)
    except pexpect.exceptions.TIMEOUT:
        pass


class Logger:
    def __init__(self, prefix):
        self.prefix = prefix
        self.raw = False;

    def write(self, data):
        if self.raw:
            if isinstance(data, bytes):
                data = data.decode('utf-8')
            sys.stdout.write(data)
        else:
            sys.stdout.write(self.prefix + repr(data)[2:-1] + '\n')

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
    tries = 0
    while True:
       try:
            ser.flush()
            ser.dtr = False
            time.sleep(0.5)
            flush()
            ser.rts = False
            time.sleep(0.5)
            ser.dtr = True
            time.sleep(0.5)
            ser.rts = True

            pe.send('U')
            pe.expect('U', searchwindowsize=1)
            break

       except pexpect.exceptions.TIMEOUT:
           tries += 1
           if tries > 3:
               raise

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
    time.sleep(0.5)
    flush()
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
