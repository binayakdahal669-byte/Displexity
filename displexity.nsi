; Displexity Language - Professional Windows Installer
; Complete installation with icons, IDE, libraries, and environment setup

!define PRODUCT_NAME "Displexity"
!define PRODUCT_VERSION "1.2.0"
!define PRODUCT_PUBLISHER "Displexity Team"
!define PRODUCT_WEB_SITE "https://github.com/displexity"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"

SetCompressor /SOLID lzma
Unicode true
!include "MUI2.nsh"
!include "FileFunc.nsh"
!include "LogicLib.nsh"

; Include language installer integration
!include "disp_programs\languages.nsh"

; Custom icons
!define MUI_ICON "exe.ico"
!define MUI_UNICON "exe.ico"
!define MUI_ABORTWARNING
!define MUI_WELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\win.bmp"

; Pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "LICENSE.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!define MUI_FINISHPAGE_RUN "$INSTDIR\bin\disp.exe"
!define MUI_FINISHPAGE_RUN_PARAMETERS "--version"
!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\docs\README.md"
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_LANGUAGE "English"

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "displexity-installer.exe"
InstallDir "$PROGRAMFILES64\Displexity"
ShowInstDetails show
RequestExecutionLevel admin

VIProductVersion "1.2.0.0"
VIAddVersionKey "ProductName" "${PRODUCT_NAME}"
VIAddVersionKey "CompanyName" "${PRODUCT_PUBLISHER}"
VIAddVersionKey "LegalCopyright" "© 2026 ${PRODUCT_PUBLISHER}"
VIAddVersionKey "FileDescription" "Displexity Programming Language Installer"
VIAddVersionKey "FileVersion" "${PRODUCT_VERSION}"

;-----------------------------------------
; INIT FUNCTION: Detect installed languages
;-----------------------------------------
Function .onInit
  Call DetectLanguages
FunctionEnd

;-----------------------------------------
; SECTION: Core Compiler
;-----------------------------------------
Section "!Compiler (Required)" SEC_COMPILER
  SectionIn RO
  SetOutPath "$INSTDIR\bin"
  
  ; Main compiler and aliases
  File "release\bin\disp.exe"
  File "release\bin\dis.exe"
  File "release\bin\dih.exe"
  File "release\bin\di.exe"
  File "release\bin\dcm.exe"
  File "release\bin\dcom.exe"
  
  ; ISO builder and aliases
  File /nonfatal "release\bin\iso_builder.exe"
  File /nonfatal "release\bin\isom.exe"
  File /nonfatal "release\bin\isob.exe"
  File /nonfatal "release\bin\bootgen.exe"
  
  ; Icons
  SetOutPath "$INSTDIR\resources"
  File "exe.ico"
  File "disp.ico"
  File "ide.ico"
  File "package.ico"
  File "runner.ico"
  File /nonfatal "release\resources\logs.ico"
  File /nonfatal "release\resources\nvim.ico"
  File /nonfatal "logs.ico"
  File /nonfatal "nvim.ico"
  File "tui.ico"
  File /nonfatal "release\resources\*.ico"
SectionEnd

;-----------------------------------------
; SECTION: Standard Libraries (.disll)
;-----------------------------------------
Section "Standard Libraries" SEC_LIBS
  SetOutPath "$INSTDIR\lib\displexity"
  File "release\lib\displexity\math.disll"
  File "release\lib\displexity\graphics.disll"
  File "release\lib\displexity\system.disll"
  File "release\lib\displexity\network.disll"
  File "release\lib\displexity\audio.disll"
  File "release\lib\displexity\crypto.disll"
  File "release\lib\displexity\window.disll"
  File "release\lib\displexity\tui.disll"
  
  ; Headers
  SetOutPath "$INSTDIR\include\displexity"
  File "release\include\displexity\math.dish"
  File "release\include\displexity\graphics.dish"
  File "release\include\displexity\system.dish"
  File "release\include\displexity\network.dish"
  File "release\include\displexity\audio.dish"
  File "release\include\displexity\crypto.dish"
  File "release\include\displexity\window.dish"
  File "release\include\displexity\tui.dish"
SectionEnd

;-----------------------------------------
; SECTION: IDE (Neovim-based)
;-----------------------------------------
Section "IDE Configuration" SEC_IDE
  ; Neovim config folder goes into bin
  SetOutPath "$INSTDIR\bin\nvim"
  File "release\bin\nvim\init.lua"
  File /nonfatal "release\bin\nvim\lazy-lock.json"
  
  ; Syntax highlighting files
  SetOutPath "$INSTDIR\bin\nvim\syntax"
  File "release\bin\nvim\syntax\disp.vim"
  File "release\bin\nvim\syntax\displexity.vim"
  
  ; Data folder with plugins (recursive copy)
  SetOutPath "$INSTDIR\bin\data"
  File /r "gui_ide\data\*.*"
  
  ; State folder
  SetOutPath "$INSTDIR\bin\state"
  File /nonfatal /r "gui_ide\state\*.*"
  
  ; Copy IDE launcher exe if exists
  SetOutPath "$INSTDIR\bin"
  File /nonfatal "gui_ide\dispe.exe"
  
  ; Create all IDE launcher aliases
  FileOpen $0 "$INSTDIR\bin\dispe.bat" w
  FileWrite $0 "@echo off$\r$\n"
  FileWrite $0 "neovide -- -u $\"%~dp0nvim\init.lua$\" %*$\r$\n"
  FileClose $0
  
  FileOpen $0 "$INSTDIR\bin\dihe.bat" w
  FileWrite $0 "@echo off$\r$\n"
  FileWrite $0 "neovide -- -u $\"%~dp0nvim\init.lua$\" %*$\r$\n"
  FileClose $0
  
  FileOpen $0 "$INSTDIR\bin\ce.bat" w
  FileWrite $0 "@echo off$\r$\n"
  FileWrite $0 "neovide -- -u $\"%~dp0nvim\init.lua$\" %*$\r$\n"
  FileClose $0
  
  FileOpen $0 "$INSTDIR\bin\cod.bat" w
  FileWrite $0 "@echo off$\r$\n"
  FileWrite $0 "neovide -- -u $\"%~dp0nvim\init.lua$\" %*$\r$\n"
  FileClose $0
  
  FileOpen $0 "$INSTDIR\bin\ed.bat" w
  FileWrite $0 "@echo off$\r$\n"
  FileWrite $0 "neovide -- -u $\"%~dp0nvim\init.lua$\" %*$\r$\n"
  FileClose $0
  
  FileOpen $0 "$INSTDIR\bin\edit.bat" w
  FileWrite $0 "@echo off$\r$\n"
  FileWrite $0 "neovide -- -u $\"%~dp0nvim\init.lua$\" %*$\r$\n"
  FileClose $0
  
  ; Plain nvim launcher
  FileOpen $0 "$INSTDIR\bin\dispnvim.bat" w
  FileWrite $0 "@echo off$\r$\n"
  FileWrite $0 "nvim -u $\"%~dp0nvim\init.lua$\" %*$\r$\n"
  FileClose $0
SectionEnd

;-----------------------------------------
; SECTION: Examples
;-----------------------------------------
Section "Examples" SEC_EXAMPLES
  SetOutPath "$INSTDIR\examples"
  File /r "release\examples\*.dis"
  
  ; Also copy lib examples if they exist
  SetOutPath "$INSTDIR\examples\lib"
  File /nonfatal "examples\lib\*.disll"
  File /nonfatal "examples\lib\*.dish"
  File /nonfatal "examples\lib\*.dis"
SectionEnd

;-----------------------------------------
; SECTION: Documentation
;-----------------------------------------
Section "Documentation" SEC_DOCS
  SetOutPath "$INSTDIR\docs"
  File "release\docs\README.md"
  File "release\docs\LANGUAGE_GUIDE.md"
  File "release\docs\API_REFERENCE.md"
  File "release\docs\STDLIB.md"
  File "release\docs\GAME_DEV.md"
  
  ; HTML Documentation
  SetOutPath "$INSTDIR\docs\html"
  File "release\docs\html\index.html"
  File "release\docs\html\style.css"
  File "release\docs\html\script.js"
SectionEnd

;-----------------------------------------
; SECTION: Library Source Code
;-----------------------------------------
Section "Library Sources" SEC_LIBSRC
  SetOutPath "$INSTDIR\lib_sources"
  File /nonfatal "lib_sources\*.dis"
SectionEnd

;-----------------------------------------
; POST INSTALL: Environment, Registry, Shortcuts
;-----------------------------------------
Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  
  ; Create README
  SetOutPath "$INSTDIR"
  FileOpen $0 "$INSTDIR\README.txt" w
  FileWrite $0 "Displexity Programming Language$\r$\n"
  FileWrite $0 "================================$\r$\n$\r$\n"
  FileWrite $0 "Quick Start:$\r$\n"
  FileWrite $0 "  1. Open Command Prompt$\r$\n"
  FileWrite $0 "  2. Type: disp --help$\r$\n"
  FileWrite $0 "  3. Compile: disp examples\hello.dis$\r$\n"
  FileWrite $0 "  4. Run: hello.exe$\r$\n$\r$\n"
  FileWrite $0 "Directories:$\r$\n"
  FileWrite $0 "  bin\          - Compiler and tools$\r$\n"
  FileWrite $0 "  lib\          - Binary libraries (.disll)$\r$\n"
  FileWrite $0 "  include\      - Header files (.dish)$\r$\n"
  FileWrite $0 "  examples\     - Example programs$\r$\n"
  FileWrite $0 "  docs\         - Documentation$\r$\n"
  FileClose $0
  
  ;----- REGISTRY -----
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "DisplayName" "${PRODUCT_NAME}"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\resources\exe.ico"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "InstallLocation" "$INSTDIR"
  
  ; Get install size
  ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
  IntFmt $0 "0x%08X" $0
  WriteRegDWORD HKLM "${PRODUCT_UNINST_KEY}" "EstimatedSize" "$0"
  
  ;----- ENVIRONMENT PATH -----
  ; Read current PATH safely
  ReadRegStr $0 HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "PATH"
  
  ; Safety check - if PATH is empty or too short, don't modify it
  StrLen $3 $0
  ${If} $3 < 50
    ; PATH seems corrupted or empty, skip modification
    MessageBox MB_OK|MB_ICONEXCLAMATION "Warning: Could not read system PATH safely. Please add Displexity bin to your PATH manually."
    Goto path_skip
  ${EndIf}
  
  ; Save original PATH for safety
  StrCpy $4 $0
  
  ; Check if already in PATH
  StrCpy $1 "$INSTDIR\bin"
  Push $4
  Push $1
  Call StrContains
  Pop $2
  StrCmp $2 "" 0 path_exists
  
  ; Add to PATH (append, don't replace)
  StrCpy $0 "$4;$INSTDIR\bin"
  WriteRegExpandStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "PATH" "$0"
  
  path_exists:
  path_skip:
  
  ; Set DISPLEXITY_HOME environment variable
  WriteRegExpandStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "DISPLEXITY_HOME" "$INSTDIR"
  
  ; Broadcast environment change
  SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
  
  ;----- FILE ASSOCIATIONS -----
  ; .dis source files
  WriteRegStr HKCR ".dis" "" "Displexity.Source"
  WriteRegStr HKCR "Displexity.Source" "" "Displexity Source File"
  WriteRegStr HKCR "Displexity.Source\DefaultIcon" "" '"$INSTDIR\resources\disp.ico"'
  WriteRegStr HKCR "Displexity.Source\shell" "" "open"
  WriteRegStr HKCR "Displexity.Source\shell\open" "" "Compile with Displexity"
  WriteRegStr HKCR "Displexity.Source\shell\open\command" "" '"$INSTDIR\bin\disp.exe" "%1"'
  WriteRegStr HKCR "Displexity.Source\shell\edit" "" "Edit"
  WriteRegStr HKCR "Displexity.Source\shell\edit\command" "" 'notepad.exe "%1"'
  
  ; .dish header files
  WriteRegStr HKCR ".dish" "" "Displexity.Header"
  WriteRegStr HKCR "Displexity.Header" "" "Displexity Header File"
  WriteRegStr HKCR "Displexity.Header\DefaultIcon" "" '"$INSTDIR\resources\disp.ico"'
  WriteRegStr HKCR "Displexity.Header\shell\edit" "" "Edit"
  WriteRegStr HKCR "Displexity.Header\shell\edit\command" "" 'notepad.exe "%1"'
  
  ; .disll library files
  WriteRegStr HKCR ".disll" "" "Displexity.Library"
  WriteRegStr HKCR "Displexity.Library" "" "Displexity Library File"
  WriteRegStr HKCR "Displexity.Library\DefaultIcon" "" '"$INSTDIR\resources\exe.ico"'
  
  ; .disp package/executable files (compiled output) - uses package.ico
  WriteRegStr HKCR ".disp" "" "Displexity.Package"
  WriteRegStr HKCR "Displexity.Package" "" "Displexity Executable"
  WriteRegStr HKCR "Displexity.Package\DefaultIcon" "" '"$INSTDIR\resources\package.ico"'
  WriteRegStr HKCR "Displexity.Package\shell" "" "open"
  WriteRegStr HKCR "Displexity.Package\shell\open" "" "Run"
  WriteRegStr HKCR "Displexity.Package\shell\open\command" "" '"$INSTDIR\bin\disp.exe" run "%1"'
  
  ; .displog log files
  WriteRegStr HKCR ".displog" "" "Displexity.Log"
  WriteRegStr HKCR "Displexity.Log" "" "Displexity Log File"
  WriteRegStr HKCR "Displexity.Log\DefaultIcon" "" '"$INSTDIR\resources\logs.ico"'
  WriteRegStr HKCR "Displexity.Log\shell\open\command" "" 'notepad.exe "%1"'
  
  ; .tui TUI bytecode files
  WriteRegStr HKCR ".tui" "" "Displexity.TUI"
  WriteRegStr HKCR "Displexity.TUI" "" "Displexity TUI Executable"
  WriteRegStr HKCR "Displexity.TUI\DefaultIcon" "" '"$INSTDIR\resources\tui.ico"'
  WriteRegStr HKCR "Displexity.TUI\shell" "" "open"
  WriteRegStr HKCR "Displexity.TUI\shell\open" "" "Run"
  WriteRegStr HKCR "Displexity.TUI\shell\open\command" "" '"$INSTDIR\bin\disp.exe" run "%1"'
  
  ; .tuy TUI bytecode files (alias for .tui)
  WriteRegStr HKCR ".tuy" "" "Displexity.TUI"
  
  ; Refresh icon cache
  System::Call 'shell32::SHChangeNotify(i 0x08000000, i 0, i 0, i 0)'
  
  ;----- SHORTCUTS -----
  CreateDirectory "$SMPROGRAMS\Displexity"
  CreateShortCut "$SMPROGRAMS\Displexity\Displexity Compiler.lnk" "$INSTDIR\bin\disp.exe" "" "$INSTDIR\resources\disp.ico"
  CreateShortCut "$SMPROGRAMS\Displexity\Examples.lnk" "$INSTDIR\examples" "" "$INSTDIR\resources\package.ico"
  CreateShortCut "$SMPROGRAMS\Displexity\Documentation.lnk" "$INSTDIR\docs" "" "$INSTDIR\resources\ide.ico"
  CreateShortCut "$SMPROGRAMS\Displexity\Uninstall.lnk" "$INSTDIR\uninst.exe"
  CreateShortCut "$DESKTOP\Displexity.lnk" "$INSTDIR\bin\disp.exe" "" "$INSTDIR\resources\disp.ico"
  
  ; IDE shortcut if installed - use nvim icon for the GUI IDE
  IfFileExists "$INSTDIR\bin\dispe.bat" 0 +2
  CreateShortCut "$SMPROGRAMS\Displexity\Displexity IDE.lnk" "$INSTDIR\bin\dispe.bat" "" "$INSTDIR\resources\nvim.ico"
SectionEnd

;-----------------------------------------
; Section Descriptions
;-----------------------------------------
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_COMPILER} "Displexity compiler (disp.exe) - Required"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_LIBS} "Standard libraries: math, graphics, system, network, audio, crypto (.disll files)"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_IDE} "Neovim-based IDE configuration with syntax highlighting"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_EXAMPLES} "Example programs demonstrating language features"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_DOCS} "Complete documentation: language guide, API reference, tutorials"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_LIBSRC} "Source code for standard libraries"
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;-----------------------------------------
; Helper Function: StrContains (FIXED - doesn't corrupt input)
;-----------------------------------------
Function StrContains
  Exch $1 ; search string
  Exch
  Exch $0 ; string to search in
  Push $2
  Push $3
  Push $4
  Push $5
  
  StrCpy $5 $0 ; Save original string
  StrLen $2 $5
  StrLen $3 $1
  
  ${If} $3 > $2
    StrCpy $0 ""
    Goto done
  ${EndIf}
  
  IntOp $2 $2 - $3
  IntOp $2 $2 + 1
  StrCpy $4 0
  
  loop:
    StrCpy $0 $5 $3 $4  ; Use saved string, not $0
    ${If} $0 == $1
      StrCpy $0 "found"
      Goto done
    ${EndIf}
    IntOp $4 $4 + 1
    ${If} $4 < $2
      Goto loop
    ${EndIf}
    StrCpy $0 ""
  done:
  Pop $5
  Pop $4
  Pop $3
  Pop $2
  Pop $1
  Exch $0
FunctionEnd

;-----------------------------------------
; UNINSTALLER
;-----------------------------------------
Section Uninstall
  ; Remove from PATH
  ReadRegStr $0 HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "PATH"
  StrCpy $1 "$INSTDIR\bin;"
  Push $0
  Push $1
  Call un.StrReplace
  Pop $0
  StrCpy $1 ";$INSTDIR\bin"
  Push $0
  Push $1
  Call un.StrReplace
  Pop $0
  WriteRegExpandStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "PATH" "$0"
  
  ; Remove DISPLEXITY_HOME
  DeleteRegValue HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "DISPLEXITY_HOME"
  
  ; Broadcast environment change
  SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
  
  ; Remove file associations
  DeleteRegKey HKCR ".dis"
  DeleteRegKey HKCR ".dish"
  DeleteRegKey HKCR ".disll"
  DeleteRegKey HKCR ".disp"
  DeleteRegKey HKCR ".displog"
  DeleteRegKey HKCR ".tui"
  DeleteRegKey HKCR ".tuy"
  DeleteRegKey HKCR "Displexity.Source"
  DeleteRegKey HKCR "Displexity.Header"
  DeleteRegKey HKCR "Displexity.Library"
  DeleteRegKey HKCR "Displexity.Package"
  DeleteRegKey HKCR "Displexity.Log"
  DeleteRegKey HKCR "Displexity.TUI"
  
  ; Refresh icon cache
  System::Call 'shell32::SHChangeNotify(i 0x08000000, i 0, i 0, i 0)'
  
  ; Remove registry
  DeleteRegKey HKLM "${PRODUCT_UNINST_KEY}"
  
  ; Remove shortcuts
  Delete "$SMPROGRAMS\Displexity\*.lnk"
  RMDir "$SMPROGRAMS\Displexity"
  Delete "$DESKTOP\Displexity.lnk"
  
  ; Remove files
  RMDir /r "$INSTDIR"
SectionEnd

; Uninstaller helper function
Function un.StrReplace
  Exch $1 ; search
  Exch
  Exch $0 ; string
  Push $2
  Push $3
  StrLen $2 $1
  StrCpy $3 ""
  loop:
    StrCpy $4 $0 $2
    StrCmp $4 $1 found
    StrCpy $4 $0 1
    StrCpy $3 "$3$4"
    StrCpy $0 $0 "" 1
    StrCmp $0 "" done loop
  found:
    StrCpy $0 $0 "" $2
    Goto loop
  done:
    StrCpy $0 $3
  Pop $3
  Pop $2
  Pop $1
  Exch $0
FunctionEnd