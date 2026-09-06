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

# Proposed names for all 256 entries, see issue #32.  Exact or nearest X11/CSS
# name where one is within dE2000 <= 5, a new name otherwise.
names = {
       0: 'Black',    1: 'Ink',    2: 'Basalt',    3: 'Slate',
       4: 'Abyss',    5: 'Twilight',    6: 'Storm',    7: 'Dusk',
       8: 'Dark Blue',    9: 'Sapphire',   10: 'Ultramarine',   11: 'Periwinkle',
      12: 'Cobalt',   13: 'Medium Blue',   14: 'Electric Blue',   15: 'Neon Blue',
      16: 'Bottle Green',   17: 'Ivy',   18: 'Fern',   19: 'Sage',
      20: 'Petrol',   21: 'Spruce',   22: 'Juniper',   23: 'Harbor',
      24: 'Oxford',   25: 'Egyptian Blue',   26: 'Denim',   27: 'Chambray',
      28: 'Baltic',   29: 'Lapis',   30: 'Royal Blue',   31: 'Blueprint',
      32: 'Green',   33: 'Shamrock',   34: 'Lime Green',   35: 'Grass',
      36: 'Malachite',   37: 'Pine',   38: 'Jade',   39: 'Mint',
      40: 'Dark Cyan',   41: 'Peacock',   42: 'Dark Turquoise',   43: 'Lagoon',
      44: 'Cerulean',   45: 'Pacific',   46: 'Bright Sky',   47: 'Ice Blue',
      48: 'Emerald',   49: 'Kelly Green',   50: 'Phosphor',   51: 'Lime',
      52: 'Signal Green',   53: 'Spearmint',   54: 'Verdant',   55: 'Spring Green',
      56: 'Persian Green',   57: 'Jungle Green',   58: 'Tiffany',   59: 'Foam',
      60: 'Light Sea Green',   61: 'Caribbean',   62: 'Cyan',   63: 'Aqua',
      64: 'Cinder',   65: 'Oxblood',   66: 'Bark',   67: 'Dusty Rose',
      68: 'Aubergine',   69: 'Damson',   70: 'Mulberry',   71: 'Heather',
      72: 'Nightshade',   73: 'Clematis',   74: 'Iris',   75: 'Slate Blue',
      76: 'Gentian',   77: 'Aconite',   78: 'Electric Violet',   79: 'Crocus',
      80: 'Peat',   81: 'Loam',   82: 'Drab',   83: 'Reed',
      84: 'Charcoal',   85: 'Graphite',   86: 'Dim Gray',   87: 'Granite',
      88: 'Midnight Blue',   89: 'Delft',   90: 'Bluebell',   91: 'Hyacinth',
      92: 'Prussian',   93: 'Marine',   94: 'Medium Slate Blue',   95: 'Larkspur',
      96: 'Apple Green',   97: 'Pesto',   98: 'Bud',   99: 'Pea',
     100: 'Forest Green',  101: 'Clover',  102: 'Meadow',  103: 'Light Green',
     104: 'Teal',  105: 'Viridian',  106: 'Medium Turquoise',  107: 'Seafoam',
     108: 'Teal Blue',  109: 'Bluebird',  110: 'Sky Blue',  111: 'Baby Blue',
     112: 'Sap',  113: 'Acid Green',  114: 'Spring Bud',  115: 'Lawn Green',
     116: 'Basil',  117: 'Leaf',  118: 'Sprout',  119: 'Pale Green',
     120: 'Kelp',  121: 'Eucalyptus',  122: 'Sea Glass',  123: 'Aquamarine',
     124: 'Ocean',  125: 'Reef',  126: 'Frost',  127: 'Ice',
     128: 'Dark Red',  129: 'Carmine',  130: 'Brick',  131: 'Cherry',
     132: 'Burgundy',  133: 'Claret',  134: 'Raspberry',  135: 'Rose',
     136: 'Dark Magenta',  137: 'Tyrian',  138: 'Medium Orchid',  139: 'Phlox',
     140: 'Imperial Purple',  141: 'Dark Violet',  142: 'Amethyst',  143: 'Lilac',
     144: 'Umber',  145: 'Mahogany',  146: 'Terracotta',  147: 'Apricot',
     148: 'Garnet',  149: 'Brown',  150: 'Indian Red',  151: 'Peony',
     152: 'Purple',  153: 'Grape',  154: 'Mallow',  155: 'Orchid',
     156: 'Purple Heart',  157: 'Dark Orchid',  158: 'Wisteria',  159: 'Bright Lavender',
     160: 'Olive',  161: 'Antique Gold',  162: 'Mustard',  163: 'Straw',
     164: 'Moss',  165: 'Brass',  166: 'Flax',  167: 'Khaki',
     168: 'Gray',  169: 'Ash',  170: 'Light Gray',  171: 'Gainsboro',
     172: 'Bluestone',  173: 'Fog',  174: 'Porcelain',  175: 'Moonlight',
     176: 'Split Pea',  177: 'Pear',  178: 'Key Lime',  179: 'Lemon Lime',
     180: 'Cactus',  181: 'Yellow Green',  182: 'Celery',  183: 'Pale Lime',
     184: 'Lichen',  185: 'Dark Sea Green',  186: 'Celadon',  187: 'Peppermint',
     188: 'Mist',  189: 'Duck Egg',  190: 'Glacier',  191: 'Light Cyan',
     192: 'Scarlet',  193: 'Poppy',  194: 'Vermilion',  195: 'Strawberry',
     196: 'Ruby',  197: 'Cardinal',  198: 'Watermelon',  199: 'Flamingo',
     200: 'Boysenberry',  201: 'Rhodamine',  202: 'Azalea',  203: 'Bubblegum',
     204: 'Plasma',  205: 'Byzantium',  206: 'Fuchsia',  207: 'Magenta',
     208: 'Rust',  209: 'Paprika',  210: 'Tomato',  211: 'Coral',
     212: 'Fire Brick',  213: 'Cranberry',  214: 'Rouge',  215: 'Light Coral',
     216: 'Byzantine',  217: 'Red Violet',  218: 'Carnation',  219: 'Cotton Candy',
     220: 'Foxglove',  221: 'Fandango',  222: 'Heliotrope',  223: 'Violet',
     224: 'Bronze',  225: 'Honey',  226: 'Amber',  227: 'Gold',
     228: 'Dark Goldenrod',  229: 'Caramel',  230: 'Buff',  231: 'Cream',
     232: 'Clay',  233: 'Rosy Brown',  234: 'Blush',  235: 'Misty Rose',
     236: 'Mauve',  237: 'Opal',  238: 'Blossom',  239: 'Petal',
     240: 'Citron',  241: 'Sulfur',  242: 'Canary',  243: 'Yellow',
     244: 'Ochre',  245: 'Olivine',  246: 'Lemon',  247: 'Butter',
     248: 'Sand',  249: 'Oat',  250: 'Light Goldenrod Yellow',  251: 'Light Yellow',
     252: 'Dark Gray',  253: 'Silver',  254: 'White Smoke',  255: 'White',
}

def print_gpl():
    print("GIMP Palette")
    print("Name: FNX-51 (RrGgBbIi)")
    print("Columns: 16")
    print("#")
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
        print(f'{RIri * 17:3d} {GIgi * 17:3d} {BIbi * 17:3d}\t0x{c:02x} {names[c]}')

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
    elif pal == 'gpl':
        print_gpl()
    elif pal == 'all':
        print_all()
    else:
        print(f'unknown palette: {pal}')
