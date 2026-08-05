# Schematic errata

Errors found in the schematic that have not (yet) been fixed in the KiCad
source. Each entry says what is wrong, how it was found, and what the correct
net is, so that `pcb/fnx51.net` can be re-exported once the sheet is edited.

---

## U13 pin 3 (Z1) is wired to `~TCC`; it should be `TCC`

**Sheet:** `video.kicad_sch` — the CD4053 that implements zoom.
**Found:** 2026-08-05, tracing the zoom path while reimplementing the scanner
on an FPGA (`../../acm/rtl/scanner.v`).

U13 is a triple 2:1 multiplexer selecting the canvas counters' count enable
between normal and zoomed. The X and Y channels pick the zoom term; the Z
channel gates it with the per-line tick:

```
X channel   X0 = +5V      X1 = ODDCOL    -> X Address Counter CE
Y channel   Y0 = +5V      Y1 = ODDROW    -> chains into C (pin 9)
Z channel   Z0 = GND      Z1 = ~TCC      -> Y Address Counter CE     <-- wrong
```

`~TCC` is the column counter's sign bit, `U19` Q3. It is HIGH for the whole
line and LOW only at the terminal count, so as wired the Y address counter is
enabled for 799 columns out of 800:

```
zoom Y off:  C = 1        ->  Y CE = ~TCC             counts 799x per line
zoom Y on:   C = ODDROW   ->  Y CE = ~TCC & ODDROW
```

The canvas Y counter must advance once per line, so the Z1 input wants the
active-high terminal count. `U12` gate 2 already produces it — it is the
inverter feeding the Row Counter's CE. With `TCC` there the two axes are
symmetric and both zoom modes come out right:

```
zoom Y off:  C = 1        ->  Y CE = TCC              once per line
zoom Y on:   C = ODDROW   ->  Y CE = TCC & ODDROW     every other line
```

which mirrors the X channel, where the tick is every pixel so no second mux
level is needed. That asymmetry is why a two-function job uses all three
switches of the 4053.

**Caught before fabrication.** No board has been laid out — only this
schematic and a partial breadboard — so nothing has been built to it. Worth
noting that the fault is invisible in the *zoomed* case and only breaks the
normal one: with zoom Y off the mux passes `~TCC` straight through and the
canvas Y counter is enabled for 799 columns of 800, so the picture could never
have come up at all. It would have been found on first power-up rather than
lurking. Nothing downstream of the netlist depends on it — `acm/` reads the
corrected sense.
