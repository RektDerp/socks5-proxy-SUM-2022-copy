﻿Unicode True
!include "MUI2.nsh"
!include LogicLib.nsh
!define APPNAME "Socks 5 Proxy"
!define COMPANYNAME "Apriorit"
!define DESCRIPTION "Course Project"
!define VERSIONMAJOR 0
!define VERSIONMINOR 0
!define VERSIONBUILD 1

!define MUI_ICON "install.ico"
#!define MUI_UNICON ""
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "header.bmp"
#!define MUI_WELCOMEFINISHPAGE_BITMAP ""
!define MUI_WELCOMEPAGE_TITLE "${APPNAME} installer"
!define MUI_WELCOMEPAGE_TEXT "${DESCRIPTION}"
!define MUI_ABORTWARNING
!define MUI_ABORTWARNING_TEXT "Are you sure want to abort installation?"
!define logo "$INSTDIR\socks5-interface.ico"
!define startLogo "$INSTDIR\start.ico"
!define stopLogo "$INSTDIR\stop.ico"

!define HELPURL http://ragalik.tk

InstallDir "$PROGRAMFILES\${APPNAME}"
OutFile "install.exe"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "gpl-3.0.rtf"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_UNPAGE_CONFIRM

Var Dialog
Var Label
Var CheckboxUser
Var CheckboxDesktop
Var CheckboxServiceControls

Var bDesktop
Var bService

Page custom nsDialogsPage
Function nsDialogsPage
    !insertmacro MUI_HEADER_TEXT "Install parameters" ""
	nsDialogs::Create 1018
	Pop $Dialog

	${If} $Dialog == error
		Abort
	${EndIf}

	${NSD_CreateLabel} 0 0 100% 12u "Select additional parameters"
	Pop $Label

    ${NSD_CreateCheckBox} 0 200 100% 6% "Create menu shortcuts only for this user (buggy)"
    Pop $CheckboxUser
    ${NSD_OnClick} $CheckboxUser setContext

    ${NSD_CreateCheckBox} 0 200 100% 6% "Create Desktop shortcuts"
    Pop $CheckboxDesktop
    ${NSD_OnClick} $CheckboxDesktop switch

    ${NSD_CreateCheckBox} 0 200 100% 6% "Create service control shortcuts"
    Pop $CheckboxServiceControls
    ${NSD_OnClick} $CheckboxServiceControls switch

	nsDialogs::Show
FunctionEnd

Var context

Function setContext
    Pop $CheckboxUser
    ${NSD_GetState} $CheckboxUser $0
    ${If} $0 == 1
        SetShellVarContext current
    ${Else}
        SetShellVarContext all
    ${EndIf}
FunctionEnd

Function switch
    ${NSD_GetState} $CheckboxDesktop $0
    StrCpy $bDesktop $0
    ${NSD_GetState} CheckboxServiceControls $0
    StrCpy $bService $0
FunctionEnd

!insertmacro MUI_UNPAGE_INSTFILES

section "install"
    setOutPath $INSTDIR
    writeUninstaller "$INSTDIR\uninstall.exe"

    file "..\build\bin\*.*"
    file "..\config.txt"
    file "socks5-interface.ico"
    file "start.ico"
    file "stop.ico"

    createShortCut "$SMPROGRAMS\${APPNAME}.lnk" "$INSTDIR\socks5-interface.exe" "" "${logo}" 0 "" "" "Apriorit project"

    ${If} $bDesktop == "1"
        createShortCut "$DESKTOP\${APPNAME}.lnk" "$INSTDIR\socks5-interface.exe" "" "${logo}" 0 "" "" "Apriorit project"
        ${If} $bService == "1"
        createShortCut "$DESKTOP\$Start {APPNAME} service.lnk" "sc.exe" "start Socks5" "${startLogo}" 0 "" "" "Service control"
        createShortCut "$DESKTOP\$Start {APPNAME} service.lnk" "sc.exe" "start Socks5" "${stopLogo}" 0 "" "" "Service control"
    ${EndIf}
    ${EndIf}

    ${If} $bService == "1"
        createShortCut "$SMPROGRAMS\$Start {APPNAME} service.lnk" "sc.exe" "start Socks5" "${startLogo}" 0 "" "" "Service control"
        createShortCut "$SMPROGRAMS\$Start {APPNAME} service.lnk" "sc.exe" "start Socks5" "${stopLogo}" 0 "" "" "Service control"
    ${EndIf}


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

    delete "$INSTDIR\*"
    delete "$SMPROGRAMS\${APPNAME}.lnk"
    rmDir $INSTDIR
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}"
sectionEnd

!insertmacro MUI_LANGUAGE "English"