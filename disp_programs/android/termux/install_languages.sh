#!/data/data/com.termux/files/usr/bin/bash
# Displexity Programming Languages Installer - Android Termux
# Updated: January 2026

echo "============================================"
echo " Displexity Language Installer - Termux"
echo "============================================"

pkg update && pkg upgrade -y

echo "Installing programming languages..."

# Essential
pkg install -y clang make cmake

# Python
pkg install -y python python-pip

# Node.js
pkg install -y nodejs

# Ruby
pkg install -y ruby

# Perl
pkg install -y perl

# PHP
pkg install -y php

# Lua
pkg install -y lua54

# Go
pkg install -y golang

# Rust
pkg install -y rust

# Java (via proot-distro)
echo "For Java, use: proot-distro install ubuntu && proot-distro login ubuntu"

# Kotlin
pkg install -y kotlin

# Nim
pkg install -y nim

# Zig
pkg install -y zig

# Erlang & Elixir
pkg install -y erlang elixir

echo ""
echo "============================================"
echo " Termux Installation Complete!"
echo "============================================"
echo ""
echo "Installed: clang, python, nodejs, ruby, perl, php, lua, go, rust, kotlin, nim, zig, erlang, elixir"
