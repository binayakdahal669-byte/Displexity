; Displexity Language Installer - Simple Version
; Professional NSIS installer script without complex dependencies

!define PRODUCT_NAME "Displexity"
!define PRODUCT_VERSION "1.0.0"
!define PRODUCT_PUBLISHER "Displexity Team"
!define PRODUCT_WEB_SITE "https://github.com/displexity/displexity"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\disp.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

; Compression
SetCompressor /SOLID lzma

; MUI 2.0 compatible
!include "MUI2.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME

; License page
!insertmacro MUI_PAGE_LICENSE "LICENSE.txt"

; Components page
!insertmacro MUI_PAGE_COMPONENTS

; Directory page
!insertmacro MUI_PAGE_DIRECTORY

; Instfiles page
!insertmacro MUI_PAGE_INSTFILES

; Finish page
!define MUI_FINISHPAGE_RUN "$INSTDIR\bin\disp.exe"
!define MUI_FINISHPAGE_RUN_PARAMETERS "--help"
!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\README.md"
!define MUI_FINISHPAGE_LINK "Visit the Displexity website"
!define MUI_FINISHPAGE_LINK_LOCATION "${PRODUCT_WEB_SITE}"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "displexity-installer.exe"
InstallDir "$PROGRAMFILES64\Displexity"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show
RequestExecutionLevel admin

; Version Information
VIProductVersion "1.0.0.0"
VIAddVersionKey "ProductName" "${PRODUCT_NAME}"
VIAddVersionKey "Comments" "Displexity Programming Language"
VIAddVersionKey "CompanyName" "${PRODUCT_PUBLISHER}"
VIAddVersionKey "LegalCopyright" "© 2026 ${PRODUCT_PUBLISHER}"
VIAddVersionKey "FileDescription" "${PRODUCT_NAME} Installer"
VIAddVersionKey "FileVersion" "${PRODUCT_VERSION}"

; Installation types
InstType "Full Installation"
InstType "Minimal Installation"
InstType "Developer Installation"

; Sections
Section "Core Components" SEC01
  SectionIn RO 1 2 3
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  
  ; Create directories
  CreateDirectory "$INSTDIR\bin"
  CreateDirectory "$INSTDIR\lib\displexity"
  CreateDirectory "$INSTDIR\include\displexity"
  CreateDirectory "$INSTDIR\examples"
  CreateDirectory "$INSTDIR\docs"
  
  ; Install compiler
  IfFileExists "bin\disp.exe" 0 +2
  File /oname=bin\disp.exe "bin\disp.exe"
  
  ; Create README
  FileOpen $0 "$INSTDIR\README.md" w
  FileWrite $0 "# Displexity Programming Language$\r$\n"
  FileWrite $0 "$\r$\n"
  FileWrite $0 "Welcome to Displexity!$\r$\n"
  FileWrite $0 "$\r$\n"
  FileWrite $0 "## Quick Start$\r$\n"
  FileWrite $0 "1. Open Command Prompt$\r$\n"
  FileWrite $0 "2. Type: disp --help$\r$\n"
  FileWrite $0 "3. Try: disp examples\hello.dis$\r$\n"
  FileWrite $0 "$\r$\n"
  FileWrite $0 "## Documentation$\r$\n"
  FileWrite $0 "- Language Guide: docs\LANGUAGE_GUIDE.md$\r$\n"
  FileWrite $0 "- API Reference: docs\API_REFERENCE.md$\r$\n"
  FileWrite $0 "- Examples: examples\$\r$\n"
  FileClose $0
SectionEnd

Section "Standard Libraries" SEC02
  SectionIn 1 3
  SetOutPath "$INSTDIR\lib\displexity"
  
  ; Copy library files if available
  IfFileExists "release\lib\displexity\math.disll" 0 +2
  File "release\lib\displexity\math.disll"
  
  IfFileExists "release\lib\displexity\graphics.disll" 0 +2
  File "release\lib\displexity\graphics.disll"
  
  IfFileExists "release\lib\displexity\system.disll" 0 +2
  File "release\lib\displexity\system.disll"
  
  IfFileExists "release\lib\displexity\network.disll" 0 +2
  File "release\lib\displexity\network.disll"
  
  IfFileExists "release\lib\displexity\audio.disll" 0 +2
  File "release\lib\displexity\audio.disll"
  
  IfFileExists "release\lib\displexity\crypto.disll" 0 +2
  File "release\lib\displexity\crypto.disll"
  
  ; Install headers
  SetOutPath "$INSTDIR\include\displexity"
  IfFileExists "release\include\displexity\*.dish" 0 +2
  File "release\include\displexity\*.dish"
SectionEnd

Section "Examples" SEC03
  SectionIn 1 3
  SetOutPath "$INSTDIR\examples"
  
  ; Copy examples if available
  IfFileExists "examples\*.*" 0 +3
  File /r "examples\*.*"
  Goto examples_done
  
  ; Create basic hello world if no examples exist
  FileOpen $0 "$INSTDIR\examples\hello.dis" w
  FileWrite $0 'print("Hello, Displexity!")$\r$\n'
  FileClose $0
  
  examples_done:
SectionEnd

Section "Documentation" SEC04
  SectionIn 1 3
  SetOutPath "$INSTDIR\docs"
  
  ; Copy documentation if available
  IfFileExists "docs\*.*" 0 +2
  File /r "docs\*.*"
SectionEnd

Section "Development Tools" SEC05
  SectionIn 3
  SetOutPath "$INSTDIR\tools"
  
  ; Copy additional tools if available
  File /nonfatal /r "tools\*.*"
  
  ; Create placeholder if no tools exist
  IfFileExists "$INSTDIR\tools\*.*" tools_done 0
  FileOpen $0 "$INSTDIR\tools\README.txt" w
  FileWrite $0 "Development tools directory$\r$\n"
  FileClose $0
  
  tools_done:
SectionEnd

Section -AdditionalIcons
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateDirectory "$SMPROGRAMS\Displexity"
  CreateShortCut "$SMPROGRAMS\Displexity\Displexity Compiler.lnk" "$INSTDIR\bin\disp.exe"
  CreateShortCut "$SMPROGRAMS\Displexity\Examples.lnk" "$INSTDIR\examples"
  CreateShortCut "$SMPROGRAMS\Displexity\Documentation.lnk" "$INSTDIR\docs"
  CreateShortCut "$SMPROGRAMS\Displexity\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$SMPROGRAMS\Displexity\Uninstall.lnk" "$INSTDIR\uninst.exe"
  CreateShortCut "$DESKTOP\Displexity.lnk" "$INSTDIR\bin\disp.exe"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\bin\disp.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\bin\disp.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
  
  ; Simple PATH addition
  ReadRegStr $0 HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "PATH"
  StrCpy $1 "$INSTDIR\bin"
  StrLen $2 $1
  StrCpy $3 $0 $2
  StrCmp $3 $1 path_exists
  StrCpy $4 $0 1 -1
  StrCmp $4 ";" 0 +2
  StrCpy $0 $0 -1
  StrCpy $0 "$0;$1"
  WriteRegExpandStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "PATH" $0
  SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
  path_exists:
  
  ; Register file associations
  WriteRegStr HKCR ".dis" "" "Displexity.Source"
  WriteRegStr HKCR "Displexity.Source" "" "Displexity Source File"
  WriteRegStr HKCR "Displexity.Source\DefaultIcon" "" "$INSTDIR\bin\disp.exe,0"
  WriteRegStr HKCR "Displexity.Source\shell\open\command" "" '"$INSTDIR\bin\disp.exe" "%1"'
  
  WriteRegStr HKCR ".dish" "" "Displexity.Header"
  WriteRegStr HKCR "Displexity.Header" "" "Displexity Header File"
  WriteRegStr HKCR "Displexity.Header\DefaultIcon" "" "$INSTDIR\bin\disp.exe,0"
  
  WriteRegStr HKCR ".disll" "" "Displexity.Library"
  WriteRegStr HKCR "Displexity.Library" "" "Displexity Library File"
  WriteRegStr HKCR "Displexity.Library\DefaultIcon" "" "$INSTDIR\bin\disp.exe,0"
  
  WriteRegStr HKCR ".disp" "" "Displexity.Package"
  WriteRegStr HKCR "Displexity.Package" "" "Displexity Package File"
  WriteRegStr HKCR "Displexity.Package\DefaultIcon" "" "$INSTDIR\bin\disp.exe,0"
  WriteRegStr HKCR "Displexity.Package\shell\open\command" "" '"$INSTDIR\bin\disp.exe" run "%1"'
SectionEnd

; Section descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC01} "Core Displexity compiler and runtime (Required)"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC02} "Standard libraries for math, graphics, system operations, networking, audio, and cryptography"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC03} "Example programs and templates to get you started"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC04} "Complete documentation including language guide and API reference"
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC05} "Additional development tools and utilities"
!insertmacro MUI_FUNCTION_DESCRIPTION_END

Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  ; Remove from PATH
  ReadRegStr $0 HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "PATH"
  StrCpy $1 "$INSTDIR\bin"
  StrLen $2 $1
  StrCpy $3 $0 $2
  StrCmp $3 $1 0 path_not_found
  StrCpy $4 $0 "" $2
  StrCpy $0 $4
  WriteRegExpandStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "PATH" $0
  SendMessage ${HWND_BROADCAST} ${WM_WININICHANGE} 0 "STR:Environment" /TIMEOUT=5000
  path_not_found:
  
  ; Remove file associations
  DeleteRegKey HKCR ".dis"
  DeleteRegKey HKCR "Displexity.Source"
  DeleteRegKey HKCR ".dish"
  DeleteRegKey HKCR "Displexity.Header"
  DeleteRegKey HKCR ".disll"
  DeleteRegKey HKCR "Displexity.Library"
  DeleteRegKey HKCR ".disp"
  DeleteRegKey HKCR "Displexity.Package"
  
  ; Remove shortcuts
  Delete "$SMPROGRAMS\Displexity\Displexity Compiler.lnk"
  Delete "$SMPROGRAMS\Displexity\Examples.lnk"
  Delete "$SMPROGRAMS\Displexity\Documentation.lnk"
  Delete "$SMPROGRAMS\Displexity\Website.lnk"
  Delete "$SMPROGRAMS\Displexity\Uninstall.lnk"
  Delete "$DESKTOP\Displexity.lnk"
  
  RMDir "$SMPROGRAMS\Displexity"
  
  ; Remove files and directories
  RMDir /r "$INSTDIR\bin"
  RMDir /r "$INSTDIR\lib"
  RMDir /r "$INSTDIR\include"
  RMDir /r "$INSTDIR\examples"
  RMDir /r "$INSTDIR\docs"
  RMDir /r "$INSTDIR\tools"
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\README.md"
  Delete "$INSTDIR\uninst.exe"
  
  RMDir "$INSTDIR"
  
  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd