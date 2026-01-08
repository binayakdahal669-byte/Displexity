# Displexity Programming Languages Bundle

This folder contains download scripts and installer integration for bundling programming languages with Displexity.

## Structure

```
disp_programs/
├── windows/           # Windows installers (.exe, .msi, .zip)
├── linux/             # Linux packages (.deb, .rpm, .tar.gz)
├── macos/             # macOS packages (.pkg, .dmg, .tar.gz)
├── android/termux/    # Termux install scripts
├── download_all_windows.ps1   # Windows download script
├── download_linux.sh          # Linux download script
├── download_macos.sh          # macOS download script
└── languages.nsh              # NSIS installer integration
```

## Supported Languages (30+)

- **Essential**: GCC/G++, Python, Java, Node.js, Rust, Go
- **Scripting**: Lua, Ruby, Perl, PHP
- **JVM**: Kotlin, Scala, Groovy, Clojure
- **Systems**: Zig, Nim, D, V
- **Functional**: Haskell, OCaml, Erlang, Elixir, Racket
- **Scientific**: Julia, R
- **Mobile/Web**: Dart, Swift
- **Enterprise**: .NET SDK, Free Pascal

## Usage

### Windows
```powershell
cd disp_programs
powershell -ExecutionPolicy Bypass -File download_all_windows.ps1
```

### Linux
```bash
cd disp_programs
chmod +x download_linux.sh
./download_linux.sh
```

### macOS
```bash
cd disp_programs
chmod +x download_macos.sh
./download_macos.sh
```

## NSIS Integration

Include `languages.nsh` in your main installer:
```nsis
!include "disp_programs\languages.nsh"
```

This adds optional language installation sections with:
- Auto-detection of existing installations
- Checkboxes for each language
- Automatic PATH configuration
