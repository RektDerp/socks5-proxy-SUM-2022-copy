Unicode True
!include "MUI2.nsh"
!include LogicLib.nsh
!define APPNAME "Socks 5 Proxy"
!define COMPANYNAME "Apriorit"
!define DESCRIPTION "Course Project"
!define VERSIONMAJOR 0
!define VERSIONMINOR 0
!define VERSIONBUILD 1

!define MUI_ICON "installer\install.ico"
#!define MUI_UNICON "installer\juju.ico"
!define MUI_HEADERIMAGE
#!define MUI_HEADERIMAGE_BITMAP "installer\cover.bmp"
#!define MUI_WELCOMEFINISHPAGE_BITMAP ""
!define MUI_WELCOMEPAGE_TITLE "${APPNAME} installer"
!define MUI_WELCOMEPAGE_TEXT "${DESCRIPTION}"
!define MUI_ABORTWARNING
!define MUI_ABORTWARNING_TEXT "Are you sure want to abort installation?"

!define HELPURL http://ragalik.tk

InstallDir "$PROGRAMFILES\${APPNAME}"
OutFile "install.exe"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "gpl-3.0.rtf"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

section "install"
    setOutPath $INSTDIR
    writeUninstaller "$INSTDIR\uninstall.exe"

    file "bin\main.exe"
    file "bin\service.exe"
    file "config.txt"
    file "lib\dll\sqlite3.dll"
    file "lib\dll\libwinpthread-1.dll"

    nsExec::Exec '"sc.exe" delete Socks5'
    nsExec::Exec '"sc.exe" create Socks5 binpath="$INSTDIR\service.exe"'

    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "DisplayName" "${APPNAME}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "UninstallString" "$\"$INSTDIR\uninstall.exe$\""
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "InstallLocation" "$\"$INSTDIR$\""
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "DisplayIcon" "$\"$INSTDIR\icon.ico$\""
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "Publisher" "${COMPANYNAME}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "HelpLink" "$\"${HELPURL}$\""
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "DisplayVersion" "${VERSIONMAJOR}.${VERSIONMINOR}.${VERSIONBUILD}"
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "NoModify" 1
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}" "NoRepair" 1
sectionEnd

section "uninstall"
    nsExec::Exec '"sc.exe" delete Socks5'

    delete "$INSTDIR\main.exe"
    delete "$INSTDIR\service.exe"
    delete "$INSTDIR\config.txt"
    delete "$INSTDIR\sqlite3.dll"
    delete "$INSTDIR\libwinpthread-1.dll"
    delete "$INSTDIR\*.db"
    delete "$INSTDIR\uninstall.exe"
    rmDir $INSTDIR
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}"
sectionEnd

!insertmacro MUI_LANGUAGE "English"
