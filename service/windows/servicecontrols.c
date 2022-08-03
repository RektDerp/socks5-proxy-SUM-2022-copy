#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include "wingdi.h"
#include "windef.h"

LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM lparam, LPARAM wparam);
HWND hWnd, hButtonStart, hButtonStop, hButtonEnable, hButtonDisable, hLabel;
HINSTANCE hI;

int main(int argc, char** argv)
{
    bool flg = true;
    MSG msg;
    wchar_t name [20] = L"kok";

    WNDCLASS wc = {0};
    wc.lpszClassName = name;
    wc.lpfnWndProc = WinProc;
    if(!RegisterClass(&wc))MessageBox(NULL,L"failed to register window class",L"error",MB_ICONERROR);

    hWnd = CreateWindowEx(0,name,L"koksos",WS_OVERLAPPED | WS_VISIBLE,CW_USEDEFAULT,CW_USEDEFAULT,120,210,NULL,NULL,wc.hInstance,0);
    if(!hWnd)MessageBox(NULL,L"failed to create window",L"error",MB_ICONERROR);

    ShowWindow(hWnd,1);
    hI = GetModuleHandle(NULL);

    while(flg)
    {
        if(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if(msg.message == WM_QUIT)flg = false;
        }
        else{

        }

    }
    return msg.wParam;
}
LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    switch (message)
    {
    case WM_CREATE:{
        hLabel = CreateWindow(L"EDIT",L"Service controls", WS_CHILD | WS_VISIBLE | ES_LEFT | ES_MULTILINE,5,5,120,40,hwnd,NULL,hI,NULL);
        hButtonStart = CreateWindowEx(WS_EX_CLIENTEDGE,L"BUTTON",L"Start",WS_CHILD | WS_VISIBLE ,5,40,105,30,hwnd,NULL,hI,NULL);
        hButtonStop = CreateWindowEx(WS_EX_CLIENTEDGE,L"BUTTON",L"Stop",WS_CHILD | WS_VISIBLE ,5,75,105,30,hwnd,NULL,hI,NULL);
        hButtonEnable = CreateWindowEx(WS_EX_CLIENTEDGE,L"BUTTON",L"Enable",WS_CHILD | WS_VISIBLE ,5,110,105,30,hwnd,NULL,hI,NULL);
        hButtonDisable = CreateWindowEx(WS_EX_CLIENTEDGE,L"BUTTON",L"Disable",WS_CHILD | WS_VISIBLE ,5,145,105,30,hwnd,NULL,hI,NULL);
    }
        break;
    case WM_COMMAND:{
        if(LOWORD(wparam) == BN_CLICKED){
            if(lparam == (LPARAM)hButtonStart)MessageBox(hWnd, L"Start", L"control", MB_OK);
            if(lparam == (LPARAM)hButtonStop)MessageBox(hWnd, L"Stop", L"control", MB_OK);
            if(lparam == (LPARAM)hButtonEnable)MessageBox(hWnd, L"Enable", L"control", MB_OK);
            if(lparam == (LPARAM)hButtonDisable)MessageBox(hWnd, L"Disable", L"control", MB_OK);
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
