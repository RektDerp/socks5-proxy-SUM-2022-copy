Unicode True
SetCompressor lzma
!include "MUI2.nsh"
!include 'FileFunc.nsh'
!include LogicLib.nsh
!define APPNAME "Socks 5 Proxy"
!define COMPANYNAME "Apriorit"
!define DESCRIPTION "Course Project"
!define VERSIONMAJOR 0
!define VERSIONMINOR 0
!define VERSIONBUILD 1
RequestExecutionLevel Admin
Name "${APPNAME}"

!insertmacro Locate
 
Var /GLOBAL switch_overwrite

!include 'MoveFileFolder.nsh'

!define MUI_ICON "install.ico"
#!define MUI_UNICON ""
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "header.bmp"
#!define MUI_WELCOMEFINISHPAGE_BITMAP ""
!define MUI_WELCOMEPAGE_TITLE "${APPNAME} installer"
!define MUI_WELCOMEPAGE_TEXT "${DESCRIPTION}"
!define MUI_ABORTWARNING
!define MUI_ABORTWARNING_TEXT "Are you sure want to abort installation?"
!define logo "$INSTDIR\statistics\socks5-interface.ico"
!define startLogo "$INSTDIR\start.ico"
!define stopLogo "$INSTDIR\stop.ico"

!define HELPURL http://ragalik.tk

InstallDir "$PROGRAMFILES\${APPNAME}"
OutFile "socks5_installer.exe"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "gpl-3.0.rtf"
!insertmacro MUI_PAGE_DIRECTORY

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

    ${NSD_CreateCheckBox} 0 78% 100% 6% "Create menu shortcuts only for this user (win 10+)"
    Pop $CheckboxUser
    ${NSD_OnClick} $CheckboxUser setContext

    ${NSD_CreateCheckBox} 0 86% 100% 6% "Create Desktop shortcuts"
    Pop $CheckboxDesktop
    ${NSD_OnClick} $CheckboxDesktop switch

    ${NSD_CreateCheckBox} 0 94% 100% 6% "Create service control shortcuts"
    Pop $CheckboxServiceControls
    ${NSD_OnClick} $CheckboxServiceControls switch
	nsDialogs::Show
FunctionEnd

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
    ${NSD_GetState} $CheckboxServiceControls $0
    StrCpy $bService $0
FunctionEnd

!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

section "install"
    setOutPath $INSTDIR
    writeUninstaller "$INSTDIR\uninstall.exe"
    
    StrCpy $switch_overwrite 0
    ;nsExec::Exec 'move "..\build\bin\socks5-interface.ico" "..\build\bin\statistics\socks5-interface.ico"'
    ;nsExec::Exec 'move "$INSTDIR\interface.exe" "$INSTDIR\statistics\interface.exe"'

    FileOpen $4 "$INSTDIR\statistics\statistics.db" w
    FileWrite $4 ""
    FileClose $4
    file /r "..\build\bin\*"
    file "..\config.txt"
    file "socks5-interface.ico"
    file "start.ico"
    !insertmacro MoveFile "$INSTDIR\interface.exe" "$INSTDIR\statistics\interface.exe"
    !insertmacro MoveFile "$INSTDIR\socks5-interface.ico" "$INSTDIR\statistics\socks5-interface.ico"
    SetOutPath $INSTDIR\statistics
    createShortCut "$SMPROGRAMS\${APPNAME}.lnk" "$INSTDIR\statistics\interface.exe" "" "${logo}" 0 "" "" "Apriorit project"
    SetOutPath $INSTDIR
    createShortCut "$SMPROGRAMS\Uninstall ${APPNAME}.lnk" "$INSTDIR\uninstall.exe" "" "${logo}" 0 "" "" "Apriorit project"
    ${If} $bDesktop == "1"
        SetOutPath $INSTDIR\statistics
        createShortCut "$DESKTOP\${APPNAME}.lnk" "$INSTDIR\statistics\interface.exe" "" "${logo}" 0 "" "" "Apriorit project"
        SetOutPath $INSTDIR
        ${If} $bService == "1"
            createShortCut "$DESKTOP\${APPNAME} service control.lnk" "$INSTDIR\servicecontrols.exe" "" "${startLogo}" 0 "" "" "Service control"
         ${EndIf}
    ${EndIf}

    ${If} $bService == "1"
        createShortCut "$SMPROGRAMS\${APPNAME} service control.lnk" "$INSTDIR\servicecontrols.exe" "" "${startLogo}" 0 "" "" "Service control"
    ${EndIf}

    nsExec::Exec '"sc.exe" delete Socks5'
    nsExec::Exec '"sc.exe" create Socks5 binpath="$INSTDIR\winservice.exe"'
    nsExec::Exec '"sc.exe" config Socks5 start=auto'

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
    nsExec::Exec '"sc.exe" stop Socks5'
    nsExec::Exec '"sc.exe" delete Socks5'
    nsExec::Exec '"taskkill" /IM main.exe /F'
    nsExec::Exec '"taskkill" /IM interface.exe /F'
    nsExec::Exec '"taskkill" /IM servicecontrols.exe /F'
    rmDir /r "$INSTDIR"
    delete "$SMPROGRAMS\${APPNAME}.lnk"
    delete "$DESKTOP\${APPNAME} service control.lnk"
    delete "$SMPROGRAMS\${APPNAME} service control.lnk"
    delete "$DESKTOP\${APPNAME}.lnk"

    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${COMPANYNAME} ${APPNAME}"
sectionEnd

!insertmacro MUI_LANGUAGE "English"
