; Displexity Language Installer Integration - Simplified
; Detects pre-installed languages

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

; Detect installed languages
Function DetectLanguages
  !insertmacro DetectLanguage "GCC" "gcc --version" $GCC_STATUS
  !insertmacro DetectLanguage "Python" "python --version" $PYTHON_STATUS
  !insertmacro DetectLanguage "Java" "java -version" $JAVA_STATUS
  !insertmacro DetectLanguage "Node.js" "node --version" $NODEJS_STATUS
  !insertmacro DetectLanguage "Rust" "rustc --version" $RUST_STATUS
  !insertmacro DetectLanguage "Go" "go version" $GO_STATUS
FunctionEnd

; Optional language notes section
SectionGroup "Optional Languages" SEC_LANGUAGES

Section /o "Language Support Notes" SEC_LANG_NOTES
  DetailPrint "Displexity can use these optional languages if installed:"
  DetailPrint "  - GCC/G++ for C/C++ compilation"
  DetailPrint "  - Python for script execution"
  DetailPrint "  - Node.js for JavaScript"
  DetailPrint "  - Java for JVM languages"
  DetailPrint "  - Rust for system programming"
  DetailPrint "  - Go for concurrent apps"
  DetailPrint ""
  DetailPrint "Install via MSYS2 or native installers:"
  DetailPrint "  pacman -S mingw-w64-x86_64-gcc"
  DetailPrint "  pacman -S mingw-w64-x86_64-python"
  DetailPrint "  pacman -S mingw-w64-x86_64-nodejs"
SectionEnd

SectionGroupEnd
