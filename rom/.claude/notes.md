# Calling Convention Refactor — Summary

## New calling convention

- **Bank 0 must always be active when calling a function.** Functions
  may rely on this at entry and must restore it before returning.
- `acc`, `r0-r7`, `B`, and both `dptr`s are callee-saved (except when
  used as return values).
- Status flags (carry etc.) are not preserved.
- Parameters and return values follow the "fastcall" style: `acc` for
  8-bit, `dptr` for pointers to data, `r0-r7` (in 16- or 32-bit pairs
  or quads) for everything else.

### Register bank allocation
- Bank 0: all normal code at call boundaries
- Bank 1: scratch — leaf functions may use it temporarily, no preservation needed
- Bank 2: reserved for interrupts
- Bank 3: reserved for userspace programs (future)

## Implementation pattern

Non-leaf functions save `r0-r7` (only those they actually clobber)
using direct-address pushes:

    push 0    ; save r0
    push 1    ; save r1
    ...
    pop 1
    pop 0

Return values pass through r0-r7 untouched (don't push/pop those).

For void functions and functions returning only via `dptr` or `acc`,
preserve `r0-r7` symmetrically.

## Files changed

Functions converted from `regbank_next/regbank_prev` to direct save:

- `sd/cmd0.inc`, `sd/cmd55.inc`, `sd/acmd41.inc` — small command wrappers
- `memory/allocate.inc` — preserves r2-r7 (r0r1 are size/result)
- `memory/release.inc` — preserves all r0-r7 (void)
- `print/int.inc` — print_int_u32 and print_int_s32 preserve all r0-r7;
  also tidied up u8/u16/s8/s16 to use direct pushes
- `fatfs/init.inc` — preserves all r0-r7 (void)

Other cleanups using direct pushes instead of the old "mov a,rN ; push acc" pattern:

- `block/init.inc`, `block/load.inc`, `fatfs/cluster.inc`,
  `util/dump.inc`, `read/hex.inc`

## Removed

- `util/regbank.inc` (the regbank_next/regbank_prev helper file)
- `.inc ../util/regbank.inc` lines in seven test .asm files

## acall vs lcall (red herring)

During the refactor the assembler emitted a bunch of "Address outside
current 2K page" warnings on `acall`/`ajmp` instructions, which made
me think the larger functions had pushed call targets past acall's
11-bit reachable range. I converted all `acall`/`ajmp` in `.inc`
files (and a few in test `.asm` files) to `lcall`/`ljmp`.

It turned out the code does still fit comfortably within 2K pages —
the warnings were almost certainly coming from intermediate build
states with unresolved symbols (as31 seems to use `0xFFFFFFFF` for
undefined symbols, which trips the page check). After reverting back
to `acall`/`ajmp`, the build is clean and all tests still pass.

Lesson: don't trust the page-range warning until you've confirmed all
referenced symbols resolve. The committed code keeps `acall`/`ajmp`.

## Verification

All 11 hardware tests under `test/` still pass (run via
`make test` against the remote Raspberry Pi harness).
