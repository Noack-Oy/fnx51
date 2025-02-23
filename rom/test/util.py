import sys, os

sys.path.append('..')
from loader import *

def test_main(run_test_fn, hex_file):
    # Determine if we're using the emulator based on the '-e' flag or if the device doesn't exist.
    using_emulator = '-e' in sys.argv
    if not os.path.exists(device):
        print(f'Device {device} not found, using emulator.')
        using_emulator = True
    try:
        if using_emulator:
            trace_file = os.path.splitext(hex_file)[0] + '.trace'
            command = f"../../emu8051/emu -serial {hex_file} 2> {trace_file}"
            pe = pexpect.spawn("bash", ["-c", command])
            pe.timeout = 1
            pe.logfile_send = Logger('> ')
            pe.logfile_read = Logger('< ')
        else:
            pe = open_serial()
            enter_bootloader()
            write_hex_file(hex_file)
            reset()
        run_test_fn(pe)
    finally:
        if using_emulator:
            pe.terminate()
        else:
            close_serial()