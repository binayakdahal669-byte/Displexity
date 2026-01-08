#!/usr/bin/env python3
"""
Generate ICO files from simple programmatic PNGs (placeholders) and from source PNGs.
Usage: python scripts/generate_icons.py --out-dir release/resources
If Pillow is not installed, the script will print instructions.
"""
import os
import sys
try:
    from PIL import Image, ImageDraw, ImageFont
except Exception:
    print("Pillow not found. Install with: pip install Pillow")
    sys.exit(1)

def make_placeholder(path, label, size=256, bg=(30,30,30), fg=(240,240,240)):
    im = Image.new('RGBA', (size, size), bg+(255,))
    draw = ImageDraw.Draw(im)
    try:
        f = ImageFont.truetype("arial.ttf", size//6)
    except Exception:
        f = ImageFont.load_default()
    w,h = draw.textsize(label, font=f)
    draw.text(((size-w)/2,(size-h)/2), label, font=f, fill=fg+(255,))
    im.save(path.replace('.ico','.png'))
    im.save(path, format='ICO', sizes=[(256,256),(128,128),(64,64),(32,32),(16,16)])

def ensure_dir(d):
    if not os.path.exists(d):
        os.makedirs(d)

def main():
    out_dir = sys.argv[1] if len(sys.argv) > 1 else 'release/resources'
    ensure_dir(out_dir)
    icons = {
        'logs.ico': 'LOG',
        'nvim.ico': 'NV',
    }
    for name,label in icons.items():
        path = os.path.join(out_dir, name)
        print('Generating', path)
        make_placeholder(path, label)

if __name__ == '__main__':
    main()
