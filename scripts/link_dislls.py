#!/usr/bin/env python3
"""
link_dislls.py

Copy matching .disll files into bin/disll for runtime discovery.

Usage:
  python scripts/link_dislls.py lib1.disll,lib2.disll
  or
  python scripts/link_dislls.py "*.disll"

If no arguments provided, nothing is done.
"""
import sys
import os
import shutil
import glob

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
OUT_DIR = os.path.join(ROOT, 'bin', 'disll')

def ensure_out():
    os.makedirs(OUT_DIR, exist_ok=True)

def copy_matches(patterns):
    ensure_out()
    copied = []
    for pat in patterns:
        # Allow comma-separated patterns
        for p in pat.split(','):
            p = p.strip()
            # If pattern contains path, join relative to root
            search = os.path.join(ROOT, p) if not any(ch in p for ch in ['*', '?']) else os.path.join(ROOT, p)
            for path in glob.glob(search, recursive=True):
                if os.path.isfile(path) and path.lower().endswith('.disll'):
                    dst = os.path.join(OUT_DIR, os.path.basename(path))
                    try:
                        shutil.copy2(path, dst)
                        copied.append(dst)
                    except Exception:
                        pass
    return copied

def main():
    if len(sys.argv) < 2:
        print('No patterns provided; nothing to copy.')
        return 0
    patterns = sys.argv[1:]
    copied = copy_matches(patterns)
    if copied:
        print('Copied dislls:')
        for c in copied:
            print('  ' + c)
    else:
        print('No .disll files found for patterns: {}'.format(patterns))
    return 0

if __name__ == '__main__':
    sys.exit(main())
