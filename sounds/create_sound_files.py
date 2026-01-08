#!/usr/bin/env python3
"""
Create sample .dish and .disll sound files for the Displexity sound system
"""

import struct
import math
import os

def create_dish_file(filename, duration=0.1, frequency=440, sample_rate=44100):
    """Create a .dish (Displexity Sound Header) file"""
    
    # Calculate samples
    sample_count = int(duration * sample_rate)
    samples = []
    
    # Generate sine wave
    for i in range(sample_count):
        t = i / sample_rate
        amplitude = 16000 * math.exp(-t * 5)  # Decay envelope
        sample = int(amplitude * math.sin(2 * math.pi * frequency * t))
        samples.append(max(-32767, min(32767, sample)))
    
    # Write DISH file
    with open(filename, 'wb') as f:
        # Magic header
        f.write(b'DISHSND1')
        
        # Header data
        f.write(struct.pack('<I', sample_rate))  # Sample rate
        f.write(struct.pack('<I', 1))            # Channels (mono)
        f.write(struct.pack('<I', sample_count)) # Sample count
        
        # Sample data
        for sample in samples:
            f.write(struct.pack('<h', sample))
    
    print(f"Created {filename}: {duration}s, {frequency}Hz, {sample_count} samples")

def create_disll_file(filename, duration=2.0, frequencies=[220, 330, 440], sample_rate=44100):
    """Create a .disll (Displexity Sound Loop Library) file"""
    
    # Calculate samples
    sample_count = int(duration * sample_rate)
    samples = []
    
    # Generate chord progression
    for i in range(sample_count):
        t = i / sample_rate
        sample = 0
        
        # Mix multiple frequencies for richer sound
        for freq in frequencies:
            amplitude = 8000 / len(frequencies)
            sample += amplitude * math.sin(2 * math.pi * freq * t)
        
        # Add some variation
        sample += 2000 * math.sin(2 * math.pi * 55 * t)  # Bass
        
        samples.append(int(max(-32767, min(32767, sample))))
    
    # Write DISLL file
    with open(filename, 'wb') as f:
        # Magic header
        f.write(b'DISLLP1\x00')  # 8 bytes with null terminator
        
        # Header data
        f.write(struct.pack('<I', sample_rate))  # Sample rate
        f.write(struct.pack('<I', 1))            # Channels (mono)
        f.write(struct.pack('<I', sample_count)) # Sample count
        
        # Sample data
        for sample in samples:
            f.write(struct.pack('<h', sample))
    
    print(f"Created {filename}: {duration}s loop, {len(frequencies)} frequencies, {sample_count} samples")

def main():
    """Create all sample sound files"""
    
    print("Creating Displexity sound files...")
    
    # Create .dish files (short sound effects)
    create_dish_file('sounds/footstep.dish', duration=0.15, frequency=200)
    create_dish_file('sounds/place.dish', duration=0.2, frequency=600)
    create_dish_file('sounds/break.dish', duration=0.25, frequency=150)
    create_dish_file('sounds/jump.dish', duration=0.3, frequency=800)
    create_dish_file('sounds/pickup.dish', duration=0.1, frequency=1200)
    
    # Create .disll files (looping background music)
    create_disll_file('sounds/background.disll', duration=4.0, frequencies=[220, 277, 330, 415])
    create_disll_file('sounds/cave.disll', duration=3.0, frequencies=[110, 138, 165])
    create_disll_file('sounds/forest.disll', duration=5.0, frequencies=[330, 415, 523, 659])
    
    print("\nSound files created successfully!")
    print("File formats:")
    print("  .dish - Displexity Sound Header (short effects)")
    print("  .disll - Displexity Sound Loop Library (background music)")
    print("\nFiles can be loaded with:")
    print("  load_sound_dish('sounds/footstep.dish')")
    print("  load_sound_disll('sounds/background.disll')")
    print("  play_sound('sounds/footstep.dish')")

if __name__ == '__main__':
    main()