#!/usr/bin/env python3

import sys

all_channels = ['Rr', 'Gg', 'Bb']

def print_primary(primary_channel):
    secondary_channels = [ x for x in all_channels if x != primary_channel ]
    pc = primary_channel
    s0 = secondary_channels[0]
    s1 = secondary_channels[1]
    sc = f'{s0}={s1}'
    print(f"| {pc}\t| {sc}\t| Ii=0\t| Ii=1\t| Ii=2\t| Ii=3\t|")
    print(f"| ----\t| ----\t| ----\t| ----\t| ----\t| ----\t|")
    for primary_value in range(1, 4):
        for secondary_value in range(0, primary_value):
            print(f"| {primary_value}\t| {secondary_value}\t", end='')
            for intensity_value in range(4):
                color = {}
                color[pc] = primary_value
                color[s0] = secondary_value
                color[s1] = secondary_value
                color['Ii'] = intensity_value
                R = color['Rr'] >> 1
                r = color['Rr']  & 1
                G = color['Gg'] >> 1
                g = color['Gg']  & 1
                B = color['Bb'] >> 1
                b = color['Bb']  & 1
                I = color['Ii'] >> 1
                i = color['Ii']  & 1
                RIri = (R << 3) + (I << 2) + (r << 1) + (i << 0)
                GIgi = (G << 3) + (I << 2) + (g << 1) + (i << 0)
                BIbi = (B << 3) + (I << 2) + (b << 1) + (i << 0)
                code = f'#{RIri:x}{GIgi:x}{BIbi:x}'
                c = (R << 7) + (r << 6) + \
                    (G << 5) + (g << 4) + \
                    (B << 3) + (b << 2) + \
                    (I << 1) + (i << 0)
                print(f'|<span style="display:inline-block;vertical-align:middle;width:1lh;height:1lh;background:{code}"></span> 0x{c:02x}\t', end='')
            print("|")

def print_secondary(secondary_channel):
    primary_channels = [ x for x in all_channels if x != secondary_channel ]
    sc = secondary_channel
    p0 = primary_channels[0]
    p1 = primary_channels[1]
    pc = f'{p0}={p1}'
    print(f"| {pc}\t| {sc}\t| Ii=0\t| Ii=1\t| Ii=2\t| Ii=3\t|")
    print(f"| ----\t| ----\t| ----\t| ----\t| ----\t| ----\t|")
    for primary_value in range(1, 4):
        for secondary_value in range(0, primary_value):
            print(f"| {primary_value}\t| {secondary_value}\t", end='')
            for intensity_value in range(4):
                color = {}
                color[p0] = primary_value
                color[p1] = primary_value
                color[sc] = secondary_value
                color['Ii'] = intensity_value
                R = color['Rr'] >> 1
                r = color['Rr']  & 1
                G = color['Gg'] >> 1
                g = color['Gg']  & 1
                B = color['Bb'] >> 1
                b = color['Bb']  & 1
                I = color['Ii'] >> 1
                i = color['Ii']  & 1
                RIri = (R << 3) + (I << 2) + (r << 1) + (i << 0)
                GIgi = (G << 3) + (I << 2) + (g << 1) + (i << 0)
                BIbi = (B << 3) + (I << 2) + (b << 1) + (i << 0)
                code = f'#{RIri:x}{GIgi:x}{BIbi:x}'
                c = (R << 7) + (r << 6) + \
                    (G << 5) + (g << 4) + \
                    (B << 3) + (b << 2) + \
                    (I << 1) + (i << 0)
                print(f'|<span style="display:inline-block;vertical-align:middle;width:1lh;height:1lh;background:{code}"></span> 0x{c:02x}\t', end='')
            print("|")

def print_tertiary(primary_channel, secondary_channel):
    tertiary_channel = [ x for x in all_channels if x != primary_channel and x != secondary_channel ][0]
    pc = primary_channel
    sc = secondary_channel
    tc = tertiary_channel
    print(f"| {pc}\t| {sc}\t| {tc}\t| Ii=0\t| Ii=1\t| Ii=2\t| Ii=3\t|")
    print(f"| ----\t| ----\t| ----\t| ----\t| ----\t| ----\t| ----\t|")
    for primary_value in range(2, 4):
        for secondary_value in range(0, primary_value):
            for tertiary_value in range(0, secondary_value):
                print(f"| {primary_value}\t| {secondary_value}\t| {tertiary_value}\t", end='')
                for intensity_value in range(4):
                    color = {}
                    color[pc] = primary_value
                    color[sc] = secondary_value
                    color[tc] = tertiary_value
                    color['Ii'] = intensity_value
                    R = color['Rr'] >> 1
                    r = color['Rr']  & 1
                    G = color['Gg'] >> 1
                    g = color['Gg']  & 1
                    B = color['Bb'] >> 1
                    b = color['Bb']  & 1
                    I = color['Ii'] >> 1
                    i = color['Ii']  & 1
                    RIri = (R << 3) + (I << 2) + (r << 1) + (i << 0)
                    GIgi = (G << 3) + (I << 2) + (g << 1) + (i << 0)
                    BIbi = (B << 3) + (I << 2) + (b << 1) + (i << 0)
                    code = f'#{RIri:x}{GIgi:x}{BIbi:x}'
                    c = (R << 7) + (r << 6) + \
                        (G << 5) + (g << 4) + \
                        (B << 3) + (b << 2) + \
                        (I << 1) + (i << 0)
                    print(f'|<span style="display:inline-block;vertical-align:middle;width:1lh;height:1lh;background:{code}"></span> 0x{c:02x}\t', end='')
                print("|")

def print_all():
    print("| #\t| R\t| r\t| G\t| g\t| B\t| b\t| I\t| i\t| Hex\t| Color\t|")
    print("|-:\t| -\t| -\t| -\t| -\t| -\t| -\t| -\t| -\t| ---\t| -----\t|")

    for c in range(256):
        R = (c >> 7) & 1
        r = (c >> 6) & 1
        G = (c >> 5) & 1
        g = (c >> 4) & 1
        B = (c >> 3) & 1
        b = (c >> 2) & 1
        I = (c >> 1) & 1
        i = (c >> 0) & 1
        RIri = (R << 3) + (I << 2) + (r << 1) + (i << 0)
        GIgi = (G << 3) + (I << 2) + (g << 1) + (i << 0)
        BIbi = (B << 3) + (I << 2) + (b << 1) + (i << 0)
        code = f'#{RIri:x}{GIgi:x}{BIbi:x}'
        print(f'| {c}\t| {R}\t| {r}\t| {G}\t| {g}\t| {B}\t| {b}\t| {I}\t| {i}\t|' +
        f'0x{c:02x}\t|<span style="display:inline-block;vertical-align:middle;width:1lh;height:1lh;background:{code}"></span> {code}\t|')

if __name__ == '__main__':
    pal = sys.argv[1] if len(sys.argv) > 1 else "all"
    if pal == 'red':
        print_primary('Rr')
    elif pal == 'green':
        print_primary('Gg')
    elif pal == 'blue':
        print_primary('Bb')
    elif pal == 'cyan':
        print_secondary('Rr')
    elif pal == 'magenta':
        print_secondary('Gg')
    elif pal == 'yellow':
        print_secondary('Bb')
    elif pal == 'amber':
        print_tertiary('Rr', 'Gg')
    elif pal == 'pink':
        print_tertiary('Rr', 'Bb')
    elif pal == 'lime':
        print_tertiary('Gg', 'Rr')
    elif pal == 'turqoise':
        print_tertiary('Gg', 'Bb')
    elif pal == 'violet':
        print_tertiary('Bb', 'Rr')
    elif pal == 'azure':
        print_tertiary('Bb', 'Gg')
    elif pal == 'all':
        print_all()
    else:
        print(f'unknown palette: {pal}')
