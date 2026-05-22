# Calling Convention Refactor — Completion Summary

Original brief: see `2026-05-22-calling-convention.txt` in this directory.

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

Non-leaf functions save the `r0-r7` they actually clobber using
direct-address pushes:

    push 0    ; save r0
    push 1    ; save r1
    ...
    pop 1
    pop 0

Return values pass through r0-r7 untouched (don't push/pop those).
For void functions and functions returning only via `dptr` or `acc`,
preserve `r0-r7` symmetrically.

## Files touched

### Phase 1 — drop the regbank mechanism

Functions converted from `regbank_next/regbank_prev` to direct save:
- `sd/cmd0.inc`, `sd/cmd55.inc`, `sd/acmd41.inc` — small command wrappers
- `memory/allocate.inc` — preserves r2-r7 (r0r1 are size/result)
- `memory/release.inc` — preserves all r0-r7 (void)
- `print/int.inc` — `print_int_u32/s32`; also tidied up the
  u8/u16/s8/s16 variants to use direct pushes instead of the old
  `mov a,rN ; push acc` shunt.
- `fatfs/init.inc` — preserves all r0-r7 (void)

Other cleanups along the same lines (direct pushes in place of the
acc-shunt pattern):
- `block/init.inc`, `block/load.inc`, `fatfs/cluster.inc`,
  `util/dump.inc`, `read/hex.inc`

Removed:
- `util/regbank.inc` (the helper file)
- `.inc ../util/regbank.inc` lines in seven test `.asm` files

### Phase 2 — bug found during review

A latent typo in `memory_allocate` was caught by extending the
memory test to exercise the "skip too-small block" path:
`mov dpl,r5` in the list-advance branch should have been
`mov dph,r5`. The walk read `curr->next` from the wrong address and
looped forever on any allocation request larger than the first free
block. Fix and matching test addition live in commits `3362ad3` and
`b2c4600`.

### Phase 3 — register allocation cleanups

With bank 0 guaranteed, several functions were carrying unnecessary
shuffling left over from the bank-agnostic era:

- `print/int.inc` (`print_int_u32` / `print_int_s32`) — kept the
  value in r0-r3 instead of moving it to r4-r7; flipped the divide
  subroutine to match. Saves the 8-instruction "shuffle to scratch"
  chain at entry.
- `sd/cmd58.inc`, `sd/cmd8.inc` — collapsed the
  `mov a,r4 / push acc / ... / xch a,r4` trick to a plain `push 4 / pop 4`.
- `sd/init.inc` — same simplification on r5.
- `print/text.inc`, `util/dptr.inc` (`dptr_copy`) — same simplification on r0.
- `memory/allocate.inc` — keep the requested size in r0r1 instead of
  shuffling it to r6r7; low-byte temp moves from r1 to r6. The exit
  copy (r4r5 → r0r1) stays because r0r1 is both input and output.
- `memory/release.inc` — keep size in r2r3; move the prev-pointer
  role to r6r7. Temp usage in the "touches-next" coalescing path
  shifts from r2/r3 to r6/r7 (same logic — those are the registers
  that are free at that point).

### Phase 4 — pointers via dptr for memory_{allocate,release}

Following the convention's "dptr for pointers to data" preference,
`memory_allocate` now returns its pointer in `dptr` instead of `r0r1`,
and `memory_release` takes its address in `dptr` instead of `r0r1`.
Size keeps its old register slot in each (`r0r1` for `allocate`,
`r2r3` for `release`).

Inside `memory_allocate` this removes the exit shuffle (no more
`r4r5 → r0r1`) and the return-value dptr no longer needs to be
saved/restored. Inside `memory_release` the cost is two `mov Rn,
direct` ops at entry to copy `dptr → r0r1` for the body, but the
test caller drops six instructions because the address survives
across the second `read_hex_32` call in `dptr` (which `read_hex_32`
preserves), instead of having to be ferried through `r4/r5`.

The other two callers (`block_init`, `fatfs_init`) get a small win
or a wash: `block_init` drops the now-redundant `mov dpl,r0 ; mov
dph,r1`, and `fatfs_init` reads `dpl/dph` instead of `r0/r1`.

## acall vs lcall (red herring)

While converting `regbank_next/regbank_prev` calls, the assembler
emitted "Address outside current 2K page" warnings on `acall`/`ajmp`
instructions. I assumed the larger function bodies had pushed call
targets past `acall`'s 11-bit reachable range and converted
everything in `.inc` files to `lcall`/`ljmp`.

This turned out to be wrong. The warnings were almost certainly
coming from intermediate build states with unresolved symbols
(`as31` appears to use `0xFFFFFFFF` for undefined symbols, which
trips the page check). After reverting back to `acall`/`ajmp`, the
build is clean and all tests still pass.

Lesson: don't trust the page-range warning until you've confirmed
all referenced symbols resolve. The committed code keeps
`acall`/`ajmp`.

## Verification

All 11 hardware tests under `rom/test/` pass (run via `make test`
against the remote Raspberry Pi harness at 192.168.1.159).
