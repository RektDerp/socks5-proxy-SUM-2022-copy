#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <wchar.h>
#include <stdint.h>
#include <stdio.h>
#include "wingdi.h"
#include "windef.h"

LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM lparam, LPARAM wparam);
HWND hWnd, hButtonStart, hButtonStop, hButtonEnable, hButtonDisable, hLabel;
HINSTANCE hI;
BOOL IsAppRunningAsAdminMode();
int windowSession ();
void wrapResult (HINSTANCE r){
    if (((uint64_t)r) < 33) {
        wchar_t error [128];
        wsprintfW(error, L"ShellExecute returned %d\n", (uint64_t)r);
        MessageBoxW(NULL, error, L"Error", MB_ICONERROR);
    }
}

int main(int argc, char** argv)
{
    if (!IsAppRunningAsAdminMode()){
        wchar_t szPath[MAX_PATH];
    if (GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath)))
    {
        SHELLEXECUTEINFO sei = { sizeof(sei) };
        sei.lpVerb = L"runas";
        sei.lpFile = szPath;
        sei.hwnd = NULL;
        sei.nShow = SW_NORMAL;
        if (!ShellExecuteEx(&sei))
        {
            DWORD dwError = GetLastError();
            if (dwError == ERROR_CANCELLED)
            {
                MessageBox(hWnd, L"Admin privilleges get failed", L"control", MB_ICONERROR);
            }
        }
        else
        {
            _exit(1);
        }
    }
    }
    if (argc > 1){
        if (strcmp(argv[1], "start")) {
            wrapResult(ShellExecute(NULL,NULL,L"C:\\Windows\\System32\\sc.exe",L"start Socks5",NULL,SW_HIDE));
        }
        else if (strcmp(argv[1], "stop")) {
            wrapResult(ShellExecute(NULL,NULL,L"C:\\Windows\\System32\\sc.exe",L"stop Socks5",NULL,SW_HIDE));
        }
        else if (strcmp(argv[1], "enable")) {
            wrapResult(ShellExecute(NULL,NULL,L"C:\\Windows\\System32\\sc.exe",L"config Socks5 start=auto",NULL,SW_HIDE));
        }
        else if (strcmp(argv[1], "disable")) {
            wrapResult(ShellExecute(NULL,NULL,L"C:\\Windows\\System32\\sc.exe",L"config Socks5 start=disabled",NULL,SW_HIDE));
        }
        else MessageBox(hWnd, L"Option not recognised", L"control", MB_OK);
    }
    else return windowSession();
    return 0;
}
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    switch (message)
    {
    case WM_CREATE:{
        hLabel = CreateWindow(L"EDIT",L"Service controls", WS_CHILD | WS_VISIBLE | ES_LEFT | ES_MULTILINE,5,5,120,40,hwnd,NULL,hI,NULL);
        hButtonStart = CreateWindowEx(WS_EX_CLIENTEDGE,L"BUTTON",L"Start",WS_CHILD | WS_VISIBLE ,10,40,105,30,hwnd,NULL,hI,NULL);
        hButtonStop = CreateWindowEx(WS_EX_CLIENTEDGE,L"BUTTON",L"Stop",WS_CHILD | WS_VISIBLE ,10,75,105,30,hwnd,NULL,hI,NULL);
        hButtonEnable = CreateWindowEx(WS_EX_CLIENTEDGE,L"BUTTON",L"Enable",WS_CHILD | WS_VISIBLE ,10,110,105,30,hwnd,NULL,hI,NULL);
        hButtonDisable = CreateWindowEx(WS_EX_CLIENTEDGE,L"BUTTON",L"Disable",WS_CHILD | WS_VISIBLE ,10,145,105,30,hwnd,NULL,hI,NULL);
    }
        break;
    case WM_COMMAND:{
        if(LOWORD(wparam) == BN_CLICKED){
            if(lparam == (LPARAM)hButtonStart){
                wrapResult(ShellExecute(NULL,NULL,L"C:\\Windows\\System32\\sc.exe",L"start Socks5",NULL,SW_HIDE));
            }
            if(lparam == (LPARAM)hButtonStop){
                wrapResult(ShellExecute(NULL,NULL,L"C:\\Windows\\System32\\sc.exe",L"stop Socks5",NULL,SW_HIDE));
            }
            if(lparam == (LPARAM)hButtonEnable){
                wrapResult(ShellExecute(NULL,NULL,L"C:\\Windows\\System32\\sc.exe",L"config Socks5 start=auto",NULL,SW_HIDE));
            }
            if(lparam == (LPARAM)hButtonDisable){
                wrapResult(ShellExecute(NULL,NULL,L"C:\\Windows\\System32\\sc.exe",L"config Socks5 start=disabled",NULL,SW_HIDE));
            }
        }
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
    break;
    case WM_PAINT:{
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));
            EndPaint(hwnd, &ps);
    }
    break;
    default:
        DefWindowProc(hwnd,message,wparam,lparam);
    break;
    }
}

int windowSession (){
    int flg = 1;
    MSG msg;
    wchar_t name [20] = L"kok";

    WNDCLASS wc = {0};
    wc.lpszClassName = name;
    wc.lpfnWndProc = WinProc;
    if(!RegisterClass(&wc))MessageBox(NULL,L"failed to register window class",L"error",MB_ICONERROR);

    hWnd = CreateWindowEx(WS_EX_CLIENTEDGE,name,L"Service", WS_SYSMENU | WS_CLIPCHILDREN,CW_USEDEFAULT,CW_USEDEFAULT,120,210,NULL,NULL,wc.hInstance,0);
    if(!hWnd)MessageBox(NULL,L"failed to create window",L"error",MB_ICONERROR);

    ShowWindow(hWnd,1);
    hI = GetModuleHandle(NULL);

    while(flg)
    {
        if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if(msg.message == WM_QUIT)flg = 0;
        }
        else{
            Sleep(10);
        }
    }
    return msg.wParam;
}

BOOL IsAppRunningAsAdminMode()
{
    BOOL fIsRunAsAdmin = FALSE;
    DWORD dwError = ERROR_SUCCESS;
    PSID pAdministratorsGroup = NULL;

    // Allocate and initialize a SID of the administrators group.
    SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
    if (!AllocateAndInitializeSid(
        &NtAuthority,
        2,
        SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS,
        0, 0, 0, 0, 0, 0,
        &pAdministratorsGroup))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

    // Determine whether the SID of administrators group is enabled in
    // the primary access token of the process.
    if (!CheckTokenMembership(NULL, pAdministratorsGroup, &fIsRunAsAdmin))
    {
        dwError = GetLastError();
        goto Cleanup;
    }

Cleanup:
    // Centralized cleanup for all allocated resources.
    if (pAdministratorsGroup)
    {
        FreeSid(pAdministratorsGroup);
        pAdministratorsGroup = NULL;
    }

    // Throw the error if something failed in the function.
    if (ERROR_SUCCESS != dwError)
    {
        return -1;
    }

    return fIsRunAsAdmin;
}
