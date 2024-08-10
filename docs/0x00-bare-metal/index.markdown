---
title: "0x00: Bare Metal"
---

If you build the FNX-51 single board computer from off-the-shelf parts,
it will be void of any software required to actually use it. This chapter
fills in that gap. Starting with nothing but a serial interface and the
rudimentary bootloader that comes with the AT89C51ED2, a machine code
monitor program is built. Then we add a driver for the video hardware and
code to interface a keyboard directly. From that point on, the computer
can be used independently, and the serial connection is no longer needed.

Unless you really want to, is not necessary to actually go through all
the steps manually yourself. Instead you can download the final code from
the website and load it into the chip using the bootloader, or you can
buy a pre-programmed chip. In any case, reading this chapter is a good
idea if you want to understand everything that is going on inside the
computer.


# The Serial Interface

In the beginning, the computer does not know how to use the video
hardware, nor can it understand signals from a keyboard. The only thing
that works is the serial interface. This requires a terminal to be
connected on the other side that provides input and ouput of plain text.
There are many option for what that could be. Therefore there are
different options available on the board, of which only one can be
installed at a time.

  1. An adapter that provides a serial connection via USB. Using this
  option you connect connect a modern PC, a raspberry PI, or even a
  smartphone or tablet using an OTG adapter.

  2. A MAX232 level shifter, required capactiors and a DB9 connector.
  With this you can connect a retro PC, old-school terminal equipment, or
  anything else that has a classic RS232 interface.

  3. Directly connect to the Rx and Tx pins using 5V logic. With this you
  can connect another microcontroller and build your own terminal with a
  keyboard and LCD, or come up with something else entirely.

Serial interfaces can work at different speeds, called "baud rate". A
common default value is 9600, which is assumed throughout this text. The
bootloader actually detects the baud rate automatically. For this it
expects the user to send a known message first. This message is the
letter "U". Once the baud rate has been detected, the bootloader should
answer by reading back the message sent. In effect this means, that you
type "U" on the terminal keyboard, and then the letter "U" appears on the
screen. It is important to understand, that you don't actually type on
the terminal screen directly. Everything that appears there is actually
the readback from the other side. If your terminal has an option called
"local echo", that needs to be turned off, or else everything appears
twice.

This is what such an exchange looks like. Lines starting with `>` show
what you type on the keyboard. Lines starting with `<` are the response
that is shown on the terminal screen. Lines that don't start with `<` or
`>` are comments to explain what is going on. The `<` and `>` markers as
well as any spaces in the messages are for readability only and don't
belong to the actual communication.

{% include_relative autobaud.ser %}

There are additional parameters that change how the serial interface
works. The defaults aussumed here are 1 start bit, 8 data bits, no parity
and at least one stop bit. We will later go into detail about how all
this works, because getting access to the serial interface is actually
the first challenge we need to solve in our own code.


# Blank Check

Before we can load our own code into the microcontroller, we need to make
sure that there isn't any code in there already. This is because it uses
a special kind of memory called "flash ROM" to store the code. This kind
of memory can be written to multiple times, but writing can only change
individual bits from 1 to 0. The only way to get back from 0 to 1 is to
erase an entire page of memory all at once, which changes all bits back
to 1, also known as the "blank" state.

{% include_relative blank-check.ser %}

