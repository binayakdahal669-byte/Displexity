; Displexity Language - 32-bit Windows Installer
; Professional installation with icons, IDE, libraries, and environment setup

!define PRODUCT_NAME "Displexity"
!define PRODUCT_VERSION "1.2.0"
!define PRODUCT_PUBLISHER "Displexity Team"
!define PRODUCT_WEB_SITE "https://github.com/displexity"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME} (32-bit)"
!define PRODUCT_ARCH "x86"

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

Name "${PRODUCT_NAME} ${PRODUCT_VERSION} (32-bit)"
OutFile "displexity-x86-installer.exe"
InstallDir "$PROGRAMFILES\Displexity"
ShowInstDetails show
RequestExecutionLevel admin

VIProductVersion "1.2.0.0"
VIAddVersionKey "ProductName" "${PRODUCT_NAME}"
VIAddVersionKey "CompanyName" "${PRODUCT_PUBLISHER}"
VIAddVersionKey "LegalCopyright" "© 2026 ${PRODUCT_PUBLISHER}"
VIAddVersionKey "FileDescription" "Displexity Programming Language (32-bit) Installer"
VIAddVersionKey "FileVersion" "${PRODUCT_VERSION}"

Function .onInit
  Call DetectLanguages
FunctionEnd

Section "!Compiler (Required)" SEC_COMPILER
  SectionIn RO
  SetOutPath "$INSTDIR\bin"
  
  File "release\bin\disp.exe"
  File "release\bin\displexitycompiler.exe"
  File /nonfatal "bin\iso_builder.exe"
  File /nonfatal "bin\bootgen.exe"
  
  SetOutPath "$INSTDIR\resources"
  File /nonfatal "exe.ico"
  File /nonfatal "disp.ico"
  File /nonfatal "ide.ico"
  File /nonfatal "package.ico"
  File /nonfatal "runner.ico"
  File /nonfatal "release\resources\logs.ico"
  File /nonfatal "release\resources\nvim.ico"
  File /nonfatal "tui.ico"
  File /nonfatal "release\resources\*.ico"
SectionEnd

Section "Standard Libraries" SEC_LIBS
  SetOutPath "$INSTDIR\lib\displexity"
  File /nonfatal "release\lib\displexity\*.disll"
  
  SetOutPath "$INSTDIR\include\displexity"
  File /nonfatal "release\include\displexity\*.dish"
SectionEnd

Section "IDE Configuration" SEC_IDE
  SetOutPath "$INSTDIR\bin\nvim"
  File /nonfatal "release\bin\nvim\init.lua"
  File /nonfatal "release\bin\nvim\lazy-lock.json"
  
  SetOutPath "$INSTDIR\bin\nvim\syntax"
  File /nonfatal "release\bin\nvim\syntax\*.vim"
  
  SetOutPath "$INSTDIR\bin\data"
  File /nonfatal /r "gui_ide\data\*.*"
  
  SetOutPath "$INSTDIR\bin\state"
  File /nonfatal /r "gui_ide\state\*.*"
  
  SetOutPath "$INSTDIR\bin"
  File /nonfatal "gui_ide\dispe.exe"
SectionEnd

Section "Examples" SEC_EXAMPLES
  SetOutPath "$INSTDIR\examples"
  File /nonfatal "release\examples\*.dis"
SectionEnd

Section "Documentation" SEC_DOCS
  SetOutPath "$INSTDIR\docs"
  File /nonfatal "release\docs\*.md"
  File /nonfatal "release\docs\*.html"
SectionEnd

Section "-Post"
  WriteUninstaller "$INSTDIR\uninst.exe"
  
  SetOutPath "$INSTDIR"
  FileOpen $0 "$INSTDIR\README.txt" w
  FileWrite $0 "Displexity Programming Language (32-bit)$\r$\n"
  FileWrite $0 "====================================$\r$\n$\r$\n"
  FileWrite $0 "Quick Start:$\r$\n"
  FileWrite $0 "  1. Open Command Prompt or PowerShell$\r$\n"
  FileWrite $0 "  2. Type: disp --help$\r$\n"
  FileWrite $0 "  3. Create hello.dis$\r$\n"
  FileWrite $0 "  4. Compile: disp hello.dis$\r$\n"
  FileWrite $0 "  5. Run: hello.exe$\r$\n"
  FileClose $0
  
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "DisplayName" "${PRODUCT_NAME} (32-bit)"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\resources\exe.ico"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "InstallLocation" "$INSTDIR"
  
  ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
  IntFmt $0 "0x%08X" $0
  WriteRegDWORD HKLM "${PRODUCT_UNINST_KEY}" "EstimatedSize" "$0"
  
  ReadRegStr $0 HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "PATH"
  StrLen $3 $0
  ${If} $3 < 50
    MessageBox MB_OK|MB_ICONEXCLAMATION "Warning: Could not read system PATH safely. Please add Displexity bin to your PATH manually."
    Goto path_skip
  ${EndIf}
  
  StrCpy $4 $0
  StrCpy $1 "$INSTDIR\bin"
  Push $4
  Push $1
  Call StrContains
  Pop $2
  StrCmp $2 "" 0 path_exists
  
  StrCpy $0 "$4;$INSTDIR\bin"
  WriteRegExpandStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "PATH" "$0"
  
  path_exists:
  path_skip:
  
  WriteRegExpandStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "DISPLEXITY_HOME" "$INSTDIR"
  SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
  
  WriteRegStr HKCR ".dis" "" "Displexity.Source"
  WriteRegStr HKCR "Displexity.Source" "" "Displexity Source File"
  WriteRegStr HKCR "Displexity.Source\DefaultIcon" "" '"$INSTDIR\resources\disp.ico"'
  WriteRegStr HKCR "Displexity.Source\shell\open\command" "" '"$INSTDIR\bin\disp.exe" "%1"'
  
  WriteRegStr HKCR ".disp" "" "Displexity.Package"
  WriteRegStr HKCR "Displexity.Package" "" "Displexity Executable"
  WriteRegStr HKCR "Displexity.Package\DefaultIcon" "" '"$INSTDIR\resources\package.ico"'
  WriteRegStr HKCR "Displexity.Package\shell\open\command" "" '"$INSTDIR\bin\disp.exe" run "%1"'
  
  WriteRegStr HKCR ".displog" "" "Displexity.Log"
  WriteRegStr HKCR "Displexity.Log" "" "Displexity Log File"
  WriteRegStr HKCR "Displexity.Log\DefaultIcon" "" '"$INSTDIR\resources\logs.ico"'
  WriteRegStr HKCR "Displexity.Log\shell\open\command" "" 'notepad.exe "%1"'
  
  System::Call 'shell32::SHChangeNotify(i 0x08000000, i 0, i 0, i 0)'
  
  CreateDirectory "$SMPROGRAMS\Displexity"
  CreateShortCut "$SMPROGRAMS\Displexity\Displexity Compiler.lnk" "$INSTDIR\bin\disp.exe" "" "$INSTDIR\resources\disp.ico"
  CreateShortCut "$SMPROGRAMS\Displexity\Examples.lnk" "$INSTDIR\examples" "" "$INSTDIR\resources\package.ico"
  CreateShortCut "$SMPROGRAMS\Displexity\Documentation.lnk" "$INSTDIR\docs" "" "$INSTDIR\resources\ide.ico"
  CreateShortCut "$SMPROGRAMS\Displexity\Uninstall.lnk" "$INSTDIR\uninst.exe"
  CreateShortCut "$DESKTOP\Displexity.lnk" "$INSTDIR\bin\disp.exe" "" "$INSTDIR\resources\disp.ico"
SectionEnd

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_COMPILER} "Displexity compiler (disp.exe) - Required"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_LIBS} "Standard libraries (math, graphics, tui, network, etc.)"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_IDE} "Neovim-based IDE configuration"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_EXAMPLES} "Example programs"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC_DOCS} "Documentation and guides"
!insertmacro MUI_FUNCTION_DESCRIPTION_END

Function StrContains
  Exch $1
  Exch
  Exch $0
  Push $2
  Push $3
  Push $4
  Push $5
  
  StrCpy $5 $0
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
    StrCpy $0 $5 $3 $4
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

Section Uninstall
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
  
  DeleteRegValue HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "DISPLEXITY_HOME"
  SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
  
  DeleteRegKey HKCR ".dis"
  DeleteRegKey HKCR ".disp"
  DeleteRegKey HKCR ".displog"
  DeleteRegKey HKCR "Displexity.Source"
  DeleteRegKey HKCR "Displexity.Package"
  DeleteRegKey HKCR "Displexity.Log"
  
  System::Call 'shell32::SHChangeNotify(i 0x08000000, i 0, i 0, i 0)'
  DeleteRegKey HKLM "${PRODUCT_UNINST_KEY}"
  Delete "$SMPROGRAMS\Displexity\*.lnk"
  RMDir "$SMPROGRAMS\Displexity"
  Delete "$DESKTOP\Displexity.lnk"
  RMDir /r "$INSTDIR"
SectionEnd

Function un.StrReplace
  Exch $1
  Exch
  Exch $0
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
