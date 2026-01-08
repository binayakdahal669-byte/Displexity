# Displexity Programming Languages Downloader - Windows PowerShell
# Downloads official installers for ALL supported languages
# Updated: January 2026 with latest versions

Write-Host "============================================" -ForegroundColor Cyan
Write-Host " Displexity Language Installer Downloader" -ForegroundColor Cyan
Write-Host "============================================" -ForegroundColor Cyan
Write-Host ""

# Create directories
$langs = @("gcc", "python", "java", "nodejs", "rust", "go", "lua", "ruby", "perl", "php", 
           "dotnet", "kotlin", "scala", "haskell", "ocaml", "zig", "nim", "crystal", 
           "julia", "r", "swift", "dart", "erlang", "elixir", "clojure", "groovy",
           "fsharp", "typescript", "coffeescript", "elm", "purescript", "racket",
           "scheme", "lisp", "prolog", "fortran", "cobol", "pascal", "ada", "d",
           "vlang", "odin", "c3", "carbon")

foreach ($lang in $langs) {
    New-Item -ItemType Directory -Force -Path "windows\$lang" | Out-Null
}

$downloads = @(
    # === 7-ZIP UTILITY (Required for extracting archives) ===
    @{Name="7-Zip Command Line"; File="windows\7za.exe"; 
      URL="https://www.7-zip.org/a/7za920.zip"},
    
    # === ESSENTIAL COMPILERS ===
    @{Name="MinGW-w64 GCC 14.2"; File="windows\gcc\mingw-w64-ucrt-x86_64.7z"; 
      URL="https://github.com/niXman/mingw-builds-binaries/releases/download/14.2.0-rt_v12-rev0/x86_64-14.2.0-release-posix-seh-ucrt-rt_v12-rev0.7z"},
    
    # === POPULAR LANGUAGES ===
    @{Name="Python 3.13.1 x64"; File="windows\python\python-3.13.1-amd64.exe"; 
      URL="https://www.python.org/ftp/python/3.13.1/python-3.13.1-amd64.exe"},
    @{Name="Python 3.13.1 x86"; File="windows\python\python-3.13.1.exe"; 
      URL="https://www.python.org/ftp/python/3.13.1/python-3.13.1.exe"},
    
    @{Name="Java OpenJDK 21 LTS"; File="windows\java\OpenJDK21U-jdk_x64_windows_hotspot.msi"; 
      URL="https://github.com/adoptium/temurin21-binaries/releases/download/jdk-21.0.5%2B11/OpenJDK21U-jdk_x64_windows_hotspot_21.0.5_11.msi"},
    
    @{Name="Node.js 22 LTS x64"; File="windows\nodejs\node-v22.12.0-x64.msi"; 
      URL="https://nodejs.org/dist/v22.12.0/node-v22.12.0-x64.msi"},
    
    @{Name="Rust (rustup)"; File="windows\rust\rustup-init.exe"; 
      URL="https://win.rustup.rs/x86_64"},
    
    @{Name="Go 1.23.4"; File="windows\go\go1.23.4.windows-amd64.msi"; 
      URL="https://go.dev/dl/go1.23.4.windows-amd64.msi"},
    
    # === SCRIPTING LANGUAGES ===
    @{Name="Lua 5.4.7"; File="windows\lua\lua-5.4.7_Win64_bin.zip"; 
      URL="https://sourceforge.net/projects/luabinaries/files/5.4.7/Tools%20Executables/lua-5.4.7_Win64_bin.zip/download"},
    
    @{Name="Ruby 3.3.6"; File="windows\ruby\rubyinstaller-3.3.6-1-x64.exe"; 
      URL="https://github.com/oneclick/rubyinstaller2/releases/download/RubyInstaller-3.3.6-1/rubyinstaller-3.3.6-1-x64.exe"},
    
    @{Name="Perl 5.38 (Strawberry)"; File="windows\perl\strawberry-perl-5.38.2.2-64bit.msi"; 
      URL="https://strawberryperl.com/download/5.38.2.2/strawberry-perl-5.38.2.2-64bit.msi"},
    
    @{Name="PHP 8.3"; File="windows\php\php-8.3.14-Win32-vs16-x64.zip"; 
      URL="https://windows.php.net/downloads/releases/php-8.3.14-Win32-vs16-x64.zip"},
    
    # === .NET ECOSYSTEM ===
    @{Name=".NET SDK 8.0"; File="windows\dotnet\dotnet-sdk-8.0.404-win-x64.exe"; 
      URL="https://download.visualstudio.microsoft.com/download/pr/dotnet-sdk-8.0.404-win-x64.exe"},
    
    # === JVM LANGUAGES ===
    @{Name="Kotlin 2.0.21"; File="windows\kotlin\kotlin-compiler-2.0.21.zip"; 
      URL="https://github.com/JetBrains/kotlin/releases/download/v2.0.21/kotlin-compiler-2.0.21.zip"},
    
    @{Name="Scala 3.5.2"; File="windows\scala\scala3-3.5.2.zip"; 
      URL="https://github.com/scala/scala3/releases/download/3.5.2/scala3-3.5.2.zip"},
    
    @{Name="Groovy 4.0.23"; File="windows\groovy\apache-groovy-sdk-4.0.23.zip"; 
      URL="https://groovy.jfrog.io/artifactory/dist-release-local/groovy-zips/apache-groovy-sdk-4.0.23.zip"},
    
    @{Name="Clojure"; File="windows\clojure\clojure-tools-1.12.0.1479.zip"; 
      URL="https://download.clojure.org/install/clojure-tools-1.12.0.1479.zip"},
    
    # === SYSTEMS LANGUAGES ===
    @{Name="Zig 0.13.0"; File="windows\zig\zig-windows-x86_64-0.13.0.zip"; 
      URL="https://ziglang.org/download/0.13.0/zig-windows-x86_64-0.13.0.zip"},
    
    @{Name="Nim 2.0.10"; File="windows\nim\nim-2.0.10_x64.zip"; 
      URL="https://nim-lang.org/download/nim-2.0.10_x64.zip"},
    
    @{Name="D (DMD)"; File="windows\d\dmd-2.109.1.exe"; 
      URL="https://downloads.dlang.org/releases/2.x/2.109.1/dmd-2.109.1.exe"},
    
    @{Name="V Language"; File="windows\vlang\v_windows.zip"; 
      URL="https://github.com/vlang/v/releases/latest/download/v_windows.zip"},
    
    # === FUNCTIONAL LANGUAGES ===
    @{Name="Haskell (GHCup)"; File="windows\haskell\ghcup-x86_64-mingw64.exe"; 
      URL="https://downloads.haskell.org/~ghcup/x86_64-mingw64-ghcup.exe"},
    
    @{Name="Erlang OTP 27"; File="windows\erlang\otp_win64_27.1.2.exe"; 
      URL="https://github.com/erlang/otp/releases/download/OTP-27.1.2/otp_win64_27.1.2.exe"},
    
    @{Name="Elixir 1.17.3"; File="windows\elixir\elixir-1.17.3-otp-27.zip"; 
      URL="https://github.com/elixir-lang/elixir/releases/download/v1.17.3/elixir-otp-27.zip"},
    
    # === SCIENTIFIC/DATA ===
    @{Name="Julia 1.11.1"; File="windows\julia\julia-1.11.1-win64.exe"; 
      URL="https://julialang-s3.julialang.org/bin/winnt/x64/1.11/julia-1.11.1-win64.exe"},
    
    @{Name="R 4.4.2"; File="windows\r\R-4.4.2-win.exe"; 
      URL="https://cran.r-project.org/bin/windows/base/R-4.4.2-win.exe"},
    
    # === MOBILE/WEB ===
    @{Name="Dart SDK 3.5"; File="windows\dart\dartsdk-windows-x64-release.zip"; 
      URL="https://storage.googleapis.com/dart-archive/channels/stable/release/3.5.4/sdk/dartsdk-windows-x64-release.zip"},
    
    @{Name="Swift 5.10"; File="windows\swift\swift-5.10.1-RELEASE-windows10.exe"; 
      URL="https://download.swift.org/swift-5.10.1-release/windows10/swift-5.10.1-RELEASE/swift-5.10.1-RELEASE-windows10.exe"},
    
    # === LISP FAMILY ===
    @{Name="Racket 8.14"; File="windows\racket\racket-8.14-x86_64-win32-cs.exe"; 
      URL="https://download.racket-lang.org/installers/8.14/racket-8.14-x86_64-win32-cs.exe"},
    
    # === LEGACY/ENTERPRISE ===
    @{Name="Free Pascal 3.2.2"; File="windows\pascal\fpc-3.2.2.i386-win32.exe"; 
      URL="https://sourceforge.net/projects/freepascal/files/Win32/3.2.2/fpc-3.2.2.i386-win32.exe/download"}
)

$count = 1
$total = ($downloads | Where-Object { $_.URL -ne $null }).Count

# First, download and extract 7-Zip
Write-Host "Downloading 7-Zip utility first (needed for extraction)..." -ForegroundColor Yellow
try {
    Invoke-WebRequest -Uri "https://www.7-zip.org/a/7zr.exe" -OutFile "windows\7zr.exe" -UseBasicParsing
    Invoke-WebRequest -Uri "https://www.7-zip.org/a/7z2408-extra.7z" -OutFile "windows\7z-extra.7z" -UseBasicParsing
    Start-Process -FilePath "windows\7zr.exe" -ArgumentList "x", "windows\7z-extra.7z", "-owindows", "-y" -Wait -NoNewWindow
    Write-Host "  -> 7-Zip extracted successfully" -ForegroundColor Gray
} catch {
    Write-Host "  -> 7-Zip download failed, some extractions may not work" -ForegroundColor Red
}

foreach ($dl in $downloads) {
    if ($dl.URL -eq $null) {
        Write-Host "[$count/$total] Skipping $($dl.Name) (bundled with other package)" -ForegroundColor Yellow
        continue
    }
    
    Write-Host "[$count/$total] Downloading $($dl.Name)..." -ForegroundColor Green
    try {
        Invoke-WebRequest -Uri $dl.URL -OutFile $dl.File -UseBasicParsing
        Write-Host "  -> Saved to $($dl.File)" -ForegroundColor Gray
    } catch {
        Write-Host "  -> FAILED: $($_.Exception.Message)" -ForegroundColor Red
    }
    $count++
}

Write-Host ""
Write-Host "============================================" -ForegroundColor Cyan
Write-Host " Download Complete!" -ForegroundColor Cyan
Write-Host "============================================" -ForegroundColor Cyan
