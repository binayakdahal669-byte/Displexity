#!/usr/bin/env python3
"""
Professional ICO file generator for Displexity
Creates 32x32 and 48x48 icons with modern designs
"""

import struct
import math

def create_ico_header(num_images):
    """Create ICO file header"""
    return struct.pack('<HHH', 0, 1, num_images)

def create_ico_entry(width, height, colors, planes, bits_per_pixel, size, offset):
    """Create ICO directory entry"""
    w = 0 if width == 256 else width
    h = 0 if height == 256 else height
    return struct.pack('<BBBBHHLL', w, h, colors, 0, planes, bits_per_pixel, size, offset)

def create_bitmap_header(width, height, bits_per_pixel):
    """Create bitmap info header"""
    return struct.pack('<LLLHHLLLLLL', 
        40,  # header size
        width, height * 2,  # width, height (doubled for ICO)
        1, bits_per_pixel,  # planes, bits per pixel
        0, 0, 0, 0, 0, 0    # compression, size, x/y pels per meter, colors used/important
    )

def blend_color(c1, c2, t):
    """Blend two colors"""
    return tuple(int(c1[i] + (c2[i] - c1[i]) * t) for i in range(len(c1)))

def create_disp_icon():
    """Create Displexity compiler icon - Modern blue 'D' with gradient"""
    width, height = 32, 32
    
    pixels = []
    center_x, center_y = 16, 16
    
    # Colors
    bg = (30, 30, 45, 255)
    primary = (0, 150, 255, 255)
    secondary = (100, 200, 255, 255)
    accent = (0, 255, 200, 255)
    
    for y in range(height):
        for x in range(width):
            # Distance from center
            dx, dy = x - center_x, y - center_y
            dist = math.sqrt(dx*dx + dy*dy)
            
            # Create rounded square background
            if abs(dx) < 14 and abs(dy) < 14:
                corner_dist = max(abs(dx) - 10, 0) + max(abs(dy) - 10, 0)
                if corner_dist < 4:
                    # Inside rounded rect
                    # Create 'D' shape
                    in_d = False
                    
                    # Left vertical bar of D
                    if x >= 8 and x <= 12 and y >= 6 and y <= 26:
                        in_d = True
                    
                    # Top horizontal of D
                    if y >= 6 and y <= 10 and x >= 8 and x <= 20:
                        in_d = True
                    
                    # Bottom horizontal of D
                    if y >= 22 and y <= 26 and x >= 8 and x <= 20:
                        in_d = True
                    
                    # Right curve of D (approximated)
                    curve_center_y = 16
                    curve_x = 20
                    curve_radius = 10
                    if x >= 18 and x <= 24:
                        curve_dist = abs(y - curve_center_y)
                        if curve_dist <= 8 and x >= 20 - (8 - curve_dist) * 0.3:
                            if x <= 24 and y >= 10 and y <= 22:
                                in_d = True
                    
                    if in_d:
                        # Gradient on D
                        t = (y - 6) / 20.0
                        blended = blend_color(primary[:3], secondary[:3], t)
                        r, g, b, a = blended[0], blended[1], blended[2], 255
                    else:
                        r, g, b, a = bg
                else:
                    r, g, b, a = 0, 0, 0, 0
            else:
                r, g, b, a = 0, 0, 0, 0
            
            pixels.extend([b, g, r, a])
    
    mask = [0] * (width * height // 8)
    bitmap_data = bytes(pixels) + bytes(mask)
    bitmap_header = create_bitmap_header(width, height, 32)
    
    ico_header = create_ico_header(1)
    ico_entry = create_ico_entry(width, height, 0, 1, 32, len(bitmap_header) + len(bitmap_data), 22)
    
    return ico_header + ico_entry + bitmap_header + bitmap_data

def create_exe_icon():
    """Create executable icon - Green gear/cog for compiled programs"""
    width, height = 32, 32
    
    pixels = []
    center_x, center_y = 16, 16
    
    for y in range(height):
        for x in range(width):
            dx, dy = x - center_x, y - center_y
            dist = math.sqrt(dx*dx + dy*dy)
            angle = math.atan2(dy, dx)
            
            # Gear shape with teeth
            teeth = 8
            tooth_angle = (2 * math.pi) / teeth
            tooth_phase = (angle % tooth_angle) / tooth_angle
            
            outer_r = 13 + 2 * (1 if tooth_phase < 0.3 or tooth_phase > 0.7 else 0)
            inner_r = 5
            
            if dist <= outer_r and dist >= inner_r:
                # Gear body - green gradient
                t = (dist - inner_r) / (outer_r - inner_r)
                r = int(50 + 100 * (1 - t))
                g = int(180 + 50 * (1 - t))
                b = int(50 + 50 * (1 - t))
                a = 255
            elif dist < inner_r and dist > 2:
                # Inner ring
                r, g, b, a = 80, 200, 80, 255
            elif dist <= 2:
                # Center dot
                r, g, b, a = 40, 120, 40, 255
            else:
                r, g, b, a = 0, 0, 0, 0
            
            pixels.extend([b, g, r, a])
    
    mask = [0] * (width * height // 8)
    bitmap_data = bytes(pixels) + bytes(mask)
    bitmap_header = create_bitmap_header(width, height, 32)
    
    ico_header = create_ico_header(1)
    ico_entry = create_ico_entry(width, height, 0, 1, 32, len(bitmap_header) + len(bitmap_data), 22)
    
    return ico_header + ico_entry + bitmap_header + bitmap_data

def create_ide_icon():
    """Create IDE icon - Purple code editor window"""
    width, height = 32, 32
    
    pixels = []
    
    for y in range(height):
        for x in range(width):
            # Window frame
            if x < 2 or x > 29 or y < 2 or y > 29:
                # Outer border - dark purple
                r, g, b, a = 60, 30, 90, 255
            elif x < 4 or x > 27 or y < 4 or y > 27:
                # Inner border - medium purple
                r, g, b, a = 100, 60, 140, 255
            elif y < 8:
                # Title bar - gradient purple
                t = (y - 4) / 4.0
                r = int(120 + 30 * t)
                g = int(80 + 20 * t)
                b = int(180 + 20 * t)
                a = 255
                # Window buttons
                if y >= 5 and y <= 7:
                    if x >= 24 and x <= 26:
                        r, g, b = 255, 100, 100  # Close
                    elif x >= 20 and x <= 22:
                        r, g, b = 255, 200, 100  # Minimize
            else:
                # Editor area - dark background
                r, g, b, a = 25, 25, 40, 255
                
                # Line numbers column
                if x >= 4 and x <= 7:
                    r, g, b = 40, 40, 60
                    # Line numbers
                    if x == 6 and y % 3 == 0 and y > 9:
                        r, g, b = 100, 100, 120
                
                # Code lines
                if x >= 9 and y > 9:
                    line_y = (y - 10) % 4
                    if line_y == 0:
                        # Keyword (purple)
                        if x >= 9 and x <= 13:
                            r, g, b = 180, 120, 220
                        # Variable (cyan)
                        elif x >= 15 and x <= 20:
                            r, g, b = 100, 200, 220
                    elif line_y == 1:
                        # String (green)
                        if x >= 11 and x <= 22:
                            r, g, b = 150, 220, 150
                    elif line_y == 2:
                        # Comment (gray)
                        if x >= 9 and x <= 25:
                            r, g, b = 100, 100, 110
            
            pixels.extend([b, g, r, a])
    
    mask = [0] * (width * height // 8)
    bitmap_data = bytes(pixels) + bytes(mask)
    bitmap_header = create_bitmap_header(width, height, 32)
    
    ico_header = create_ico_header(1)
    ico_entry = create_ico_entry(width, height, 0, 1, 32, len(bitmap_header) + len(bitmap_data), 22)
    
    return ico_header + ico_entry + bitmap_header + bitmap_data

def create_package_icon():
    """Create .disp package icon - Orange/gold box with D"""
    width, height = 32, 32
    
    pixels = []
    center_x, center_y = 16, 16
    
    for y in range(height):
        for x in range(width):
            # 3D box shape
            in_box = False
            on_edge = False
            
            # Front face
            if x >= 6 and x <= 26 and y >= 10 and y <= 28:
                in_box = True
                # Edge highlight
                if x == 6 or x == 26 or y == 10 or y == 28:
                    on_edge = True
            
            # Top face (parallelogram)
            if y >= 4 and y < 10:
                top_left = 6 + (10 - y)
                top_right = 26 + (10 - y)
                if x >= top_left and x <= top_right:
                    in_box = True
                    if y == 4 or x == top_left or x == top_right:
                        on_edge = True
            
            if in_box:
                if on_edge:
                    r, g, b, a = 180, 120, 40, 255  # Dark gold edge
                else:
                    # Gradient orange/gold
                    t = (y - 4) / 24.0
                    r = int(255 - 30 * t)
                    g = int(180 - 40 * t)
                    b = int(50 + 20 * t)
                    a = 255
                    
                    # 'D' letter on front
                    if y >= 14 and y <= 24 and x >= 12 and x <= 22:
                        # Simple D shape
                        if x <= 14:  # Left bar
                            r, g, b = 40, 30, 20
                        elif y <= 16 or y >= 22:  # Top/bottom
                            if x <= 20:
                                r, g, b = 40, 30, 20
                        elif x >= 19 and x <= 21:  # Right curve
                            if y >= 16 and y <= 22:
                                r, g, b = 40, 30, 20
            else:
                r, g, b, a = 0, 0, 0, 0
            
            pixels.extend([b, g, r, a])
    
    mask = [0] * (width * height // 8)
    bitmap_data = bytes(pixels) + bytes(mask)
    bitmap_header = create_bitmap_header(width, height, 32)
    
    ico_header = create_ico_header(1)
    ico_entry = create_ico_entry(width, height, 0, 1, 32, len(bitmap_header) + len(bitmap_data), 22)
    
    return ico_header + ico_entry + bitmap_header + bitmap_data

def create_runner_icon():
    """Create runner/terminal icon - Blue play button with terminal"""
    width, height = 32, 32
    
    pixels = []
    center_x, center_y = 16, 16
    
    for y in range(height):
        for x in range(width):
            # Terminal window background
            if x >= 3 and x <= 28 and y >= 5 and y <= 27:
                # Border
                if x <= 5 or x >= 26 or y <= 7 or y >= 25:
                    r, g, b, a = 60, 60, 80, 255
                else:
                    # Terminal background
                    r, g, b, a = 20, 20, 30, 255
                    
                    # Play triangle in center
                    play_cx, play_cy = 16, 16
                    # Triangle points: left(10,10), left(10,22), right(22,16)
                    # Check if point is inside triangle
                    px, py = x, y
                    
                    # Barycentric coordinates for triangle
                    x1, y1 = 10, 10
                    x2, y2 = 10, 22
                    x3, y3 = 22, 16
                    
                    denom = (y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3)
                    if denom != 0:
                        a_coord = ((y2 - y3) * (px - x3) + (x3 - x2) * (py - y3)) / denom
                        b_coord = ((y3 - y1) * (px - x3) + (x1 - x3) * (py - y3)) / denom
                        c_coord = 1 - a_coord - b_coord
                        
                        if a_coord >= 0 and b_coord >= 0 and c_coord >= 0:
                            # Inside play button - cyan/blue gradient
                            r = int(0 + 100 * (1 - a_coord))
                            g = int(200 + 55 * b_coord)
                            b = int(255)
                            a = 255
            else:
                r, g, b, a = 0, 0, 0, 0
            
            pixels.extend([b, g, r, a])
    
    mask = [0] * (width * height // 8)
    bitmap_data = bytes(pixels) + bytes(mask)
    bitmap_header = create_bitmap_header(width, height, 32)
    
    ico_header = create_ico_header(1)
    ico_entry = create_ico_entry(width, height, 0, 1, 32, len(bitmap_header) + len(bitmap_data), 22)
    
    return ico_header + ico_entry + bitmap_header + bitmap_data

# Create all icons
icons = {
    'disp.ico': create_disp_icon(),
    'exe.ico': create_exe_icon(),
    'ide.ico': create_ide_icon(),
    'package.ico': create_package_icon(),
    'runner.ico': create_runner_icon(),
}

for name, data in icons.items():
    with open(name, 'wb') as f:
        f.write(data)
    print(f"Created: {name}")

# Also copy to release/resources
import os
os.makedirs('release/resources', exist_ok=True)
for name, data in icons.items():
    with open(f'release/resources/{name}', 'wb') as f:
        f.write(data)
    print(f"Created: release/resources/{name}")

print("\nAll icons created successfully!")
print("Icons: disp.ico (compiler), exe.ico (executables), ide.ico (IDE),")
print("       package.ico (.disp files), runner.ico (runner/terminal)")
