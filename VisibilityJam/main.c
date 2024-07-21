#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <stdio.h>
#include <assert.h>
#include "messageArray.h"

// Setup for using Common Controls
#pragma comment(linker,                                     \
                "\"/manifestdependency:type='Win32' "       \
                "name='Microsoft.Windows.Common-Controls' " \
                "version='6.0.0.0' "                        \
                "processorArchitecture='*' "                \
                "publicKeyToken='6595b64144ccf1df' "        \
                "language='*'\"")

#define	ID_EDITCONTROL	111
#define WM_CUSTOM_LOGWM	(WM_APP + 1)

HWND globalHWndLog;

LRESULT wndProc(
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam
)
{
	LRESULT result = 0;

	switch (msg)
	{
		case WM_DESTROY:
		{
			DestroyWindow(hWnd);
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
		}
	}

	// Print information about the message
	WindowMessage message = getMessage(msg);
		assert(msg == message.id);
		wchar_t messageText[256];
		_snwprintf(
			messageText,
			256,
			L"%s (%d)\r\n",
			message.name,
			msg
		);
		SendMessageW(
			globalHWndLog, 
			WM_CUSTOM_LOGWM,
			0, 
			(LPARAM)messageText
		);

	return result;
}

LRESULT logWndProc(
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam
)
{
	static HWND hWndEdit;

	switch (msg)
	{
		case WM_CREATE:
		{
			// Create edit control
			hWndEdit = CreateWindowEx(
				0, 
				L"EDIT", 
				NULL, 
				WS_CHILD | WS_VISIBLE | WS_VSCROLL |
				ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
				0, 
				0,
				0,
				0,   
				hWnd, 
				(HMENU)ID_EDITCONTROL,
				(HINSTANCE) GetWindowLongPtrW(
					hWnd, 
					GWLP_HINSTANCE
				),
				NULL
			);
			
			// Control is read only
			SendMessageW(
				hWndEdit, 
				EM_SETREADONLY,
				TRUE, 
				0
			);

			// Increase text limit
			SendMessageW(
				hWndEdit, 
				EM_SETLIMITTEXT, 
				423849203, // TODO: Better number
				0
			);
			return 0;
		}
		case WM_SIZE:
		{
			// Edit control size is window size
			MoveWindow(
				hWndEdit,
				0, 
				0,                  
				LOWORD(lParam),    
				HIWORD(lParam),     
				TRUE);              
			return 0;
		}
		case WM_CUSTOM_LOGWM:
		{
			int index = GetWindowTextLength(hWndEdit);
			SetFocus(hWndEdit);
			// Set selection to ned of text
			SendMessageW(
				hWndEdit, 
				EM_SETSEL, 
				(WPARAM)index, 
				(LPARAM)index);
			// Replace all text in the selection
			SendMessageW(
				hWndEdit,
				EM_REPLACESEL,
				0, 
				lParam
			);
			return 0;
		}
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		default:
		{
			return DefWindowProcW(
				hWnd,
				msg,
				wParam,
				lParam
			);
		}
	}
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

	// Calculate window positions
	int logX = workAreaRect.left;
	int logY = workAreaRect.top;
	int logHeight = workAreaHeight - logY;
	int logWidth = 325;

	int windowSize = 400;
	int windowX =
		((workAreaWidth - logWidth) / 2) +
		logWidth -
		(windowSize / 2) +
		(logX / 2);
	int windowY =
		(workAreaHeight / 2) -
		(windowSize / 2) +
		(logY / 2);

	// Initialize main window
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

	// Initialize log window
	WNDCLASSW wndClassLog = { 0 };
	wndClassLog.lpfnWndProc = logWndProc;
	wndClassLog.hInstance = hInstance;
	wndClassLog.lpszClassName =
		L"LogWindow";
	wndClassLog.hbrBackground =
		GetStockObject(WHITE_BRUSH);

	RegisterClassW(&wndClassLog);

	HWND hWndLog = CreateWindowExW(
		0,
		wndClassLog.lpszClassName,
		L"Message Log",
		WS_OVERLAPPEDWINDOW,
		logX,
		logY,
		logWidth,
		logHeight,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	ShowWindow(
		hWndLog,
		nCmdShow
	);

	globalHWndLog = hWndLog;

	// Message loop
	MSG msg;
	while (GetMessageW(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
}