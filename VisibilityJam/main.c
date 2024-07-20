/*
 * TODOS:
 * - Fix positioning with top/right/left taskbar
 * - Better way to wait for console to close
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
    LRESULT result = 0;

    // Print information about the message
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
    // Get work area (size excluding taskbar)
    RECT workAreaRect;
    SystemParametersInfoW(
        SPI_GETWORKAREA,
        0,
        &workAreaRect,
        0
    );
    int workAreaWidth = workAreaRect.right;
    int workAreaHeight = workAreaRect.bottom;

    // Get screen size
    int screenWidth =
        GetSystemMetrics(SM_CXSCREEN);
    int screenHeight =
        GetSystemMetrics(SM_CYSCREEN);

    // Calculate window positions
    int consoleHeight = workAreaHeight;
    int consoleWidth = 300;

    int windowSize = 400;
    int remainingWorkAreaWidth =
        workAreaWidth - consoleWidth;
    int windowX =
        (remainingWorkAreaWidth / 2) +
        consoleWidth -
        (windowSize / 2);
    int windowY =
        (workAreaHeight / 2) -
        (windowSize / 2);

    // Initialize window
    WNDCLASSW wndClass = { 0 };
    wndClass.lpfnWndProc = wndProc;
    wndClass.hInstance = hInstance;
    wndClass.lpszClassName =
        L"VisibilityJam";
    wndClass.hbrBackground =
        GetStockObject(WHITE_BRUSH);

    RegisterClassW(&wndClass);

    HWND hWnd = CreateWindowExW(
        0,
        wndClass.lpszClassName,
        L"Visibility Jam",
        WS_OVERLAPPEDWINDOW,
        windowX,
        windowY,
        windowSize,
        windowSize,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    ShowWindow(
        hWnd,
        nCmdShow
    );

    // Initialize console
    BOOL result = AllocConsole();
    assert(result);

    // Redirect STDOUT to our console
    FILE* fresult = freopen(
        "CONOUT$",
        "w",
        stdout
    );
    assert(fresult);

    // Position console
    SetWindowPos(
        GetConsoleWindow(),
        hWnd,
        0,
        0,
        consoleWidth,
        consoleHeight,
        0
    );

    // Message loop
    // This is deliberately broken because we 
    // do not want the program to exit when the
    // window is closed, only when the console
    // closes
    MSG msg;
    while (1)
    {
        result = GetMessageW(
            &msg,
            NULL,
            0,
            0
        );
        if (result > 0)
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
}