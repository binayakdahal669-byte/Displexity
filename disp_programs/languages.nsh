; Displexity Language Installer Integration
; Include this in displexity.nsi for optional language installation

!include "EnvVarUpdate.nsh"

; === 7-ZIP UTILITY (Required for extracting archives) ===
Section "-7-Zip Utility" SEC_7ZIP
  SetOutPath "$INSTDIR"
  File /nonfatal "disp_programs\windows\7za.exe"
  File /nonfatal "disp_programs\windows\7za.dll"
SectionEnd

; Language detection macros
!macro DetectLanguage NAME CMD VAR
  nsExec::ExecToStack '${CMD}'
  Pop $0
  Pop $1
  ${If} $0 == 0
    StrCpy ${VAR} "installed"
  ${Else}
    StrCpy ${VAR} "not_installed"
  ${EndIf}
!macroend

; Variables for language detection
Var GCC_STATUS
Var PYTHON_STATUS
Var JAVA_STATUS
Var NODEJS_STATUS
Var RUST_STATUS
Var GO_STATUS
Var LUA_STATUS
Var RUBY_STATUS
Var PERL_STATUS
Var PHP_STATUS
Var DOTNET_STATUS
Var KOTLIN_STATUS
Var ZIG_STATUS
Var NIM_STATUS
Var JULIA_STATUS
Var DART_STATUS
Var ERLANG_STATUS
Var ELIXIR_STATUS
Var R_STATUS
Var SWIFT_STATUS
Var CLOJURE_STATUS
Var SCALA_STATUS
Var HASKELL_STATUS
Var GROOVY_STATUS
Var D_STATUS
Var VLANG_STATUS
Var OCAML_STATUS
Var RACKET_STATUS
Var PASCAL_STATUS
Var FORTRAN_STATUS

; Detect installed languages
Function DetectLanguages
  !insertmacro DetectLanguage "GCC" "gcc --version" $GCC_STATUS
  !insertmacro DetectLanguage "Python" "python --version" $PYTHON_STATUS
  !insertmacro DetectLanguage "Java" "java -version" $JAVA_STATUS
  !insertmacro DetectLanguage "Node.js" "node --version" $NODEJS_STATUS
  !insertmacro DetectLanguage "Rust" "rustc --version" $RUST_STATUS
  !insertmacro DetectLanguage "Go" "go version" $GO_STATUS
  !insertmacro DetectLanguage "Lua" "lua -v" $LUA_STATUS
  !insertmacro DetectLanguage "Ruby" "ruby --version" $RUBY_STATUS
  !insertmacro DetectLanguage "Perl" "perl --version" $PERL_STATUS
  !insertmacro DetectLanguage "PHP" "php --version" $PHP_STATUS
  !insertmacro DetectLanguage ".NET" "dotnet --version" $DOTNET_STATUS
  !insertmacro DetectLanguage "Kotlin" "kotlin -version" $KOTLIN_STATUS
  !insertmacro DetectLanguage "Zig" "zig version" $ZIG_STATUS
  !insertmacro DetectLanguage "Nim" "nim --version" $NIM_STATUS
  !insertmacro DetectLanguage "Julia" "julia --version" $JULIA_STATUS
  !insertmacro DetectLanguage "Dart" "dart --version" $DART_STATUS
  !insertmacro DetectLanguage "Erlang" "erl -version" $ERLANG_STATUS
  !insertmacro DetectLanguage "Elixir" "elixir --version" $ELIXIR_STATUS
  !insertmacro DetectLanguage "R" "Rscript --version" $R_STATUS
  !insertmacro DetectLanguage "Swift" "swift --version" $SWIFT_STATUS
  !insertmacro DetectLanguage "Clojure" "clj --version" $CLOJURE_STATUS
  !insertmacro DetectLanguage "Scala" "scala --version" $SCALA_STATUS
  !insertmacro DetectLanguage "Haskell" "ghc --version" $HASKELL_STATUS
  !insertmacro DetectLanguage "Groovy" "groovy --version" $GROOVY_STATUS
  !insertmacro DetectLanguage "D" "dmd --version" $D_STATUS
  !insertmacro DetectLanguage "V" "v version" $VLANG_STATUS
  !insertmacro DetectLanguage "OCaml" "ocaml -version" $OCAML_STATUS
  !insertmacro DetectLanguage "Racket" "racket --version" $RACKET_STATUS
  !insertmacro DetectLanguage "Pascal" "fpc -h" $PASCAL_STATUS
  !insertmacro DetectLanguage "Fortran" "gfortran --version" $FORTRAN_STATUS
FunctionEnd

; === LANGUAGE INSTALLATION SECTIONS ===

SectionGroup "Programming Languages" SEC_LANGUAGES

Section /o "GCC/G++ Compiler" SEC_GCC
  ${If} $GCC_STATUS == "not_installed"
    SetOutPath "$INSTDIR\languages\gcc"
    File /r "disp_programs\windows\gcc\*.*"
    nsExec::ExecToLog '"$INSTDIR\7za.exe" x "$INSTDIR\languages\gcc\mingw-w64-ucrt-x86_64.7z" -o"$INSTDIR\languages\gcc"'
    ${EnvVarUpdate} $0 "PATH" "A" "HKLM" "$INSTDIR\languages\gcc\mingw64\bin"
    DetailPrint "GCC installed and added to PATH"
  ${Else}
    DetailPrint "GCC already installed, skipping"
  ${EndIf}
SectionEnd

Section /o "Python" SEC_PYTHON
  ${If} $PYTHON_STATUS == "not_installed"
    SetOutPath "$TEMP"
    File "disp_programs\windows\python\python-3.13.1-amd64.exe"
    nsExec::ExecToLog '"$TEMP\python-3.13.1-amd64.exe" /quiet InstallAllUsers=1 PrependPath=1'
    DetailPrint "Python installed"
  ${Else}
    DetailPrint "Python already installed, skipping"
  ${EndIf}
SectionEnd

Section /o "Java (OpenJDK)" SEC_JAVA
  ${If} $JAVA_STATUS == "not_installed"
    SetOutPath "$TEMP"
    File "disp_programs\windows\java\OpenJDK21U-jdk_x64_windows_hotspot.msi"
    nsExec::ExecToLog 'msiexec /i "$TEMP\OpenJDK21U-jdk_x64_windows_hotspot.msi" /quiet ADDLOCAL=FeatureMain,FeatureEnvironment,FeatureJarFileRunWith,FeatureJavaHome'
    DetailPrint "Java installed"
  ${Else}
    DetailPrint "Java already installed, skipping"
  ${EndIf}
SectionEnd

Section /o "Node.js" SEC_NODEJS
  ${If} $NODEJS_STATUS == "not_installed"
    SetOutPath "$TEMP"
    File "disp_programs\windows\nodejs\node-v22.12.0-x64.msi"
    nsExec::ExecToLog 'msiexec /i "$TEMP\node-v22.12.0-x64.msi" /quiet'
    DetailPrint "Node.js installed"
  ${Else}
    DetailPrint "Node.js already installed, skipping"
  ${EndIf}
SectionEnd

Section /o "Rust" SEC_RUST
  ${If} $RUST_STATUS == "not_installed"
    SetOutPath "$TEMP"
    File "disp_programs\windows\rust\rustup-init.exe"
    nsExec::ExecToLog '"$TEMP\rustup-init.exe" -y --default-toolchain stable'
    ${EnvVarUpdate} $0 "PATH" "A" "HKCU" "$PROFILE\.cargo\bin"
    DetailPrint "Rust installed"
  ${Else}
    DetailPrint "Rust already installed, skipping"
  ${EndIf}
SectionEnd

Section /o "Go" SEC_GO
  ${If} $GO_STATUS == "not_installed"
    SetOutPath "$TEMP"
    File "disp_programs\windows\go\go1.23.4.windows-amd64.msi"
    nsExec::ExecToLog 'msiexec /i "$TEMP\go1.23.4.windows-amd64.msi" /quiet'
    DetailPrint "Go installed"
  ${Else}
    DetailPrint "Go already installed, skipping"
  ${EndIf}
SectionEnd

Section /o "Lua" SEC_LUA
  ${If} $LUA_STATUS == "not_installed"
    SetOutPath "$INSTDIR\languages\lua"
    File /r "disp_programs\windows\lua\*.*"
    ${EnvVarUpdate} $0 "PATH" "A" "HKLM" "$INSTDIR\languages\lua"
    DetailPrint "Lua installed and added to PATH"
  ${Else}
    DetailPrint "Lua already installed, skipping"
  ${EndIf}
SectionEnd

Section /o "Ruby" SEC_RUBY
  ${If} $RUBY_STATUS == "not_installed"
    SetOutPath "$TEMP"
    File "disp_programs\windows\ruby\rubyinstaller-3.3.6-1-x64.exe"
    nsExec::ExecToLog '"$TEMP\rubyinstaller-3.3.6-1-x64.exe" /verysilent /tasks="assocfiles,modpath"'
    DetailPrint "Ruby installed"
  ${Else}
    DetailPrint "Ruby already installed, skipping"
  ${EndIf}
SectionEnd

Section /o "Perl" SEC_PERL
  ${If} $PERL_STATUS == "not_installed"
    SetOutPath "$TEMP"
    File "disp_programs\windows\perl\strawberry-perl-5.38.2.2-64bit.msi"
    nsExec::ExecToLog 'msiexec /i "$TEMP\strawberry-perl-5.38.2.2-64bit.msi" /quiet'
    DetailPrint "Perl installed"
  ${Else}
    DetailPrint "Perl already installed, skipping"
  ${EndIf}
SectionEnd

Section /o "PHP" SEC_PHP
  ${If} $PHP_STATUS == "not_installed"
    SetOutPath "$INSTDIR\languages\php"
    File /r "disp_programs\windows\php\*.*"
    ${EnvVarUpdate} $0 "PATH" "A" "HKLM" "$INSTDIR\languages\php"
    DetailPrint "PHP installed and added to PATH"
  ${Else}
    DetailPrint "PHP already installed, skipping"
  ${EndIf}
SectionEnd

Section /o ".NET SDK" SEC_DOTNET
  ${If} $DOTNET_STATUS == "not_installed"
    SetOutPath "$TEMP"
    File "disp_programs\windows\dotnet\dotnet-sdk-8.0.404-win-x64.exe"
    nsExec::ExecToLog '"$TEMP\dotnet-sdk-8.0.404-win-x64.exe" /install /quiet /norestart'
    DetailPrint ".NET SDK installed"
  ${Else}
    DetailPrint ".NET already installed, skipping"
  ${EndIf}
SectionEnd

Section /o "Kotlin" SEC_KOTLIN
  ${If} $KOTLIN_STATUS == "not_installed"
    SetOutPath "$INSTDIR\languages\kotlin"
    File /r "disp_programs\windows\kotlin\*.*"
    nsExec::ExecToLog '"$INSTDIR\7za.exe" x "$INSTDIR\languages\kotlin\kotlin-compiler-2.0.21.zip" -o"$INSTDIR\languages\kotlin"'
    ${EnvVarUpdate} $0 "PATH" "A" "HKLM" "$INSTDIR\languages\kotlin\kotlinc\bin"
    DetailPrint "Kotlin installed and added to PATH"
  ${Else}
    DetailPrint "Kotlin already installed, skipping"
  ${EndIf}
SectionEnd

Section /o "Zig" SEC_ZIG
  ${If} $ZIG_STATUS == "not_installed"
    SetOutPath "$INSTDIR\languages\zig"
    File /r "disp_programs\windows\zig\*.*"
    nsExec::ExecToLog '"$INSTDIR\7za.exe" x "$INSTDIR\languages\zig\zig-windows-x86_64-0.13.0.zip" -o"$INSTDIR\languages\zig"'
    ${EnvVarUpdate} $0 "PATH" "A" "HKLM" "$INSTDIR\languages\zig\zig-windows-x86_64-0.13.0"
    DetailPrint "Zig installed and added to PATH"
  ${Else}
    DetailPrint "Zig already installed, skipping"
  ${EndIf}
SectionEnd

Section /o "Nim" SEC_NIM
  ${If} $NIM_STATUS == "not_installed"
    SetOutPath "$INSTDIR\languages\nim"
    File /r "disp_programs\windows\nim\*.*"
    nsExec::ExecToLog '"$INSTDIR\7za.exe" x "$INSTDIR\languages\nim\nim-2.0.10_x64.zip" -o"$INSTDIR\languages\nim"'
    ${EnvVarUpdate} $0 "PATH" "A" "HKLM" "$INSTDIR\languages\nim\nim-2.0.10\bin"
    DetailPrint "Nim installed and added to PATH"
  ${Else}
    DetailPrint "Nim already installed, skipping"
  ${EndIf}
SectionEnd

Section /o "Julia" SEC_JULIA
  ${If} $JULIA_STATUS == "not_installed"
    SetOutPath "$TEMP"
    File "disp_programs\windows\julia\julia-1.11.1-win64.exe"
    nsExec::ExecToLog '"$TEMP\julia-1.11.1-win64.exe" /S /D=$INSTDIR\languages\julia'
    ${EnvVarUpdate} $0 "PATH" "A" "HKLM" "$INSTDIR\languages\julia\bin"
    DetailPrint "Julia installed"
  ${Else}
    DetailPrint "Julia already installed, skipping"
  ${EndIf}
SectionEnd

Section /o "Dart" SEC_DART
  ${If} $DART_STATUS == "not_installed"
    SetOutPath "$INSTDIR\languages\dart"
    File /r "disp_programs\windows\dart\*.*"
    nsExec::ExecToLog '"$INSTDIR\7za.exe" x "$INSTDIR\languages\dart\dartsdk-windows-x64-release.zip" -o"$INSTDIR\languages\dart"'
    ${EnvVarUpdate} $0 "PATH" "A" "HKLM" "$INSTDIR\languages\dart\dart-sdk\bin"
    DetailPrint "Dart installed and added to PATH"
  ${Else}
    DetailPrint "Dart already installed, skipping"
  ${EndIf}
SectionEnd

Section /o "Erlang" SEC_ERLANG
  ${If} $ERLANG_STATUS == "not_installed"
    SetOutPath "$TEMP"
    File "disp_programs\windows\erlang\otp_win64_27.1.2.exe"
    nsExec::ExecToLog '"$TEMP\otp_win64_27.1.2.exe" /S'
    DetailPrint "Erlang installed"
  ${Else}
    DetailPrint "Erlang already installed, skipping"
  ${EndIf}
SectionEnd

Section /o "Elixir" SEC_ELIXIR
  ${If} $ELIXIR_STATUS == "not_installed"
    SetOutPath "$INSTDIR\languages\elixir"
    File /r "disp_programs\windows\elixir\*.*"
    nsExec::ExecToLog '"$INSTDIR\7za.exe" x "$INSTDIR\languages\elixir\elixir-1.17.3-otp-27.zip" -o"$INSTDIR\languages\elixir"'
    ${EnvVarUpdate} $0 "PATH" "A" "HKLM" "$INSTDIR\languages\elixir\bin"
    DetailPrint "Elixir installed and added to PATH"
  ${Else}
    DetailPrint "Elixir already installed, skipping"
  ${EndIf}
SectionEnd

Section /o "R Language" SEC_R
  ${If} $R_STATUS == "not_installed"
    SetOutPath "$TEMP"
    File "disp_programs\windows\r\R-4.4.2-win.exe"
    nsExec::ExecToLog '"$TEMP\R-4.4.2-win.exe" /VERYSILENT /NORESTART'
    DetailPrint "R installed"
  ${Else}
    DetailPrint "R already installed, skipping"
  ${EndIf}
SectionEnd

Section /o "Swift" SEC_SWIFT
  ${If} $SWIFT_STATUS == "not_installed"
    SetOutPath "$TEMP"
    File "disp_programs\windows\swift\swift-5.10.1-RELEASE-windows10.exe"
    nsExec::ExecToLog '"$TEMP\swift-5.10.1-RELEASE-windows10.exe" /quiet'
    DetailPrint "Swift installed"
  ${Else}
    DetailPrint "Swift already installed, skipping"
  ${EndIf}
SectionEnd

Section /o "Scala" SEC_SCALA
  ${If} $SCALA_STATUS == "not_installed"
    SetOutPath "$INSTDIR\languages\scala"
    File /r "disp_programs\windows\scala\*.*"
    nsExec::ExecToLog '"$INSTDIR\7za.exe" x "$INSTDIR\languages\scala\scala3-3.5.2.zip" -o"$INSTDIR\languages\scala"'
    ${EnvVarUpdate} $0 "PATH" "A" "HKLM" "$INSTDIR\languages\scala\scala3-3.5.2\bin"
    DetailPrint "Scala installed and added to PATH"
  ${Else}
    DetailPrint "Scala already installed, skipping"
  ${EndIf}
SectionEnd

Section /o "Haskell (GHCup)" SEC_HASKELL
  ${If} $HASKELL_STATUS == "not_installed"
    SetOutPath "$TEMP"
    File "disp_programs\windows\haskell\ghcup-x86_64-mingw64.exe"
    nsExec::ExecToLog '"$TEMP\ghcup-x86_64-mingw64.exe" install ghc'
    DetailPrint "Haskell GHCup installed"
  ${Else}
    DetailPrint "Haskell already installed, skipping"
  ${EndIf}
SectionEnd

Section /o "Groovy" SEC_GROOVY
  ${If} $GROOVY_STATUS == "not_installed"
    SetOutPath "$INSTDIR\languages\groovy"
    File /r "disp_programs\windows\groovy\*.*"
    nsExec::ExecToLog '"$INSTDIR\7za.exe" x "$INSTDIR\languages\groovy\apache-groovy-sdk-4.0.23.zip" -o"$INSTDIR\languages\groovy"'
    ${EnvVarUpdate} $0 "PATH" "A" "HKLM" "$INSTDIR\languages\groovy\groovy-4.0.23\bin"
    DetailPrint "Groovy installed and added to PATH"
  ${Else}
    DetailPrint "Groovy already installed, skipping"
  ${EndIf}
SectionEnd

Section /o "D Language (DMD)" SEC_D
  ${If} $D_STATUS == "not_installed"
    SetOutPath "$TEMP"
    File "disp_programs\windows\d\dmd-2.109.1.exe"
    nsExec::ExecToLog '"$TEMP\dmd-2.109.1.exe" /S'
    DetailPrint "D Language installed"
  ${Else}
    DetailPrint "D already installed, skipping"
  ${EndIf}
SectionEnd

Section /o "V Language" SEC_VLANG
  ${If} $VLANG_STATUS == "not_installed"
    SetOutPath "$INSTDIR\languages\vlang"
    File /r "disp_programs\windows\vlang\*.*"
    nsExec::ExecToLog '"$INSTDIR\7za.exe" x "$INSTDIR\languages\vlang\v_windows.zip" -o"$INSTDIR\languages\vlang"'
    ${EnvVarUpdate} $0 "PATH" "A" "HKLM" "$INSTDIR\languages\vlang\v"
    DetailPrint "V Language installed and added to PATH"
  ${Else}
    DetailPrint "V already installed, skipping"
  ${EndIf}
SectionEnd

Section /o "Racket" SEC_RACKET
  ${If} $RACKET_STATUS == "not_installed"
    SetOutPath "$TEMP"
    File "disp_programs\windows\racket\racket-8.14-x86_64-win32-cs.exe"
    nsExec::ExecToLog '"$TEMP\racket-8.14-x86_64-win32-cs.exe" /S'
    DetailPrint "Racket installed"
  ${Else}
    DetailPrint "Racket already installed, skipping"
  ${EndIf}
SectionEnd

Section /o "Free Pascal" SEC_PASCAL
  ${If} $PASCAL_STATUS == "not_installed"
    SetOutPath "$TEMP"
    File "disp_programs\windows\pascal\fpc-3.2.2.i386-win32.exe"
    nsExec::ExecToLog '"$TEMP\fpc-3.2.2.i386-win32.exe" /VERYSILENT /NORESTART'
    DetailPrint "Free Pascal installed"
  ${Else}
    DetailPrint "Free Pascal already installed, skipping"
  ${EndIf}
SectionEnd

SectionGroupEnd
