/*
 * TODOS:
 * - Add WM_USER processing
 * - Do not close console window when main
 *       window closes
 */
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <stdio.h>
#include <assert.h>
#include "messageArray.h"

LRESULT wndProc(
    HWND hWnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam
)
{
    // Assume we have handled the message
    LRESULT result = 0;

    WindowMessage message = getMessage(msg);
    assert(msg == message.id);
    wprintf(
        L"%s (%d)\n",
        message.name,
        msg
    );

    switch (msg)
    {
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            break;
        }
        default:
        {
            result = DefWindowProcW(
                hWnd,
                msg,
                wParam,
                lParam
            );
            break;
        }
    }
    return result;
}

int CALLBACK wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow
)
{
    HWND hWndDesktop = GetDesktopWindow();
    RECT desktopRect;
    GetWindowRect(hWndDesktop, &desktopRect);
    POINT desktopWidth = 
    { 
        desktopRect.top,
        desktopRect.bottom 
    };
    ScreenToClient(
        hWndDesktop,
        &desktopWidth
    );
    

    // Window stuff
    WNDCLASSW wndClass = { 0 };
    wndClass.lpfnWndProc = wndProc;
    wndClass.hInstance = hInstance;
    wndClass.lpszClassName = L"VisibilityJam";
    wndClass.hbrBackground = 
        GetStockObject(WHITE_BRUSH);

    RegisterClassW(&wndClass);

    HWND hWnd = CreateWindowExW(
        0,
        wndClass.lpszClassName,
        L"Visibility Jam",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        500,
        500,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    // Console stuff
    BOOL result = AllocConsole();
    assert(result);

    // Redirect STDOUT to our console
    FILE* fresult = freopen(
        "CONOUT$", 
        "w", 
        stdout
    );
    assert(fresult);

    SetWindowPos(
        GetConsoleWindow(), 
        HWND_TOPMOST, 
        0,
        0, 
        300, 
        300, //desktopWidth.x - desktopWidth.y,
        0
    );

    ShowWindow(
        hWnd, 
        nCmdShow
    );

    // Message loop
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    // Wait for user to close the console
    while (GetConsoleWindow() != NULL)
    { }
}