#!/usr/bin/env python3
"""Create TUI icon for Displexity TUI executables"""
from PIL import Image, ImageDraw, ImageFont
import os

def create_tui_icon():
    sizes = [16, 32, 48, 64, 128, 256]
    images = []
    
    for size in sizes:
        img = Image.new('RGBA', (size, size), (0, 0, 0, 0))
        draw = ImageDraw.Draw(img)
        
        # Background - dark purple/magenta gradient feel
        margin = size // 16
        draw.rounded_rectangle(
            [margin, margin, size - margin, size - margin],
            radius=size // 8,
            fill=(128, 0, 128, 255)  # Purple
        )
        
        # Inner rectangle - terminal look
        inner_margin = size // 6
        draw.rounded_rectangle(
            [inner_margin, inner_margin, size - inner_margin, size - inner_margin],
            radius=size // 12,
            fill=(40, 40, 60, 255)  # Dark blue-gray
        )
        
        # Draw "TUI" text or terminal cursor
        if size >= 48:
            # Draw terminal-style lines
            line_y = inner_margin + size // 6
            line_height = size // 12
            for i in range(3):
                line_width = size // 3 if i == 2 else size // 2
                draw.rectangle(
                    [inner_margin + size // 10, line_y + i * (line_height + size // 20),
                     inner_margin + size // 10 + line_width, line_y + i * (line_height + size // 20) + line_height // 2],
                    fill=(0, 255, 128, 255)  # Green terminal text
                )
            # Cursor
            cursor_y = line_y + 3 * (line_height + size // 20)
            draw.rectangle(
                [inner_margin + size // 10, cursor_y,
                 inner_margin + size // 10 + size // 8, cursor_y + line_height],
                fill=(255, 255, 255, 200)  # White cursor
            )
        else:
            # Simple > prompt for small sizes
            center = size // 2
            draw.polygon(
                [(center - size // 6, center - size // 8),
                 (center + size // 8, center),
                 (center - size // 6, center + size // 8)],
                fill=(0, 255, 128, 255)
            )
        
        images.append(img)
    
    # Save as ICO
    images[0].save('tui.ico', format='ICO', sizes=[(s, s) for s in sizes], append_images=images[1:])
    print("Created tui.ico")
    
    # Copy to release/resources if exists
    if os.path.exists('release/resources'):
        images[0].save('release/resources/tui.ico', format='ICO', sizes=[(s, s) for s in sizes], append_images=images[1:])
        print("Copied to release/resources/tui.ico")

if __name__ == '__main__':
    create_tui_icon()
