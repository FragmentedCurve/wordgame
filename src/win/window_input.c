#include <windows.h>
#include <stdbool.h>

#include "common.h"
#include "mm.h"

#define INDEX_INPUT   0
#define INDEX_LETTERS 1 * sizeof(LONG_PTR)

#define Y_OFFSET (1 - (RATIO_ACTION + RATIO_STATUS + RATIO_INPUT))

static LRESULT CALLBACK InputWindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
	case WM_SIZE:
		{
			HWND input_text = (HWND) GetWindowLongPtr(window, INDEX_INPUT);
			HWND letters_text = (HWND) GetWindowLongPtr(window, INDEX_LETTERS);
						
			HWND parent = GetParent(window);
			WINDOWSIZE psize = GetWindowSize(parent);
			WINDOWSIZE wsize = GetWindowSize(window);
			UINT y = psize.height * Y_OFFSET;

			wsize.width = psize.width;
			wsize.height = psize.height * RATIO_INPUT;
			
			MoveWindow(window, 0, y, wsize.width, wsize.height, FALSE);
			MoveWindow(input_text, 0, 0, wsize.width, wsize.height / 2, TRUE);
			MoveWindow(letters_text, 0, wsize.height / 2, wsize.width, wsize.height / 2, TRUE);
		} break;
	case WM_PLAYERINPUT:
		{
			RECT rc;
			HWND input_text = (HWND) GetWindowLongPtr(window, INDEX_INPUT);
			HBRUSH bg_brush = CreateSolidBrush(BGCOLOR); // TODO: Does this have to be freed?
			HDC dc = GetDC(input_text);
			char *str = (char *) lparam;
			GetClientRect(input_text, &rc);
			FillRect(dc, &rc, bg_brush);
			SetWindowText(input_text, str);
			ReleaseDC(input_text, dc);
			DeleteObject(bg_brush);
		} break;
	case WM_GAMELETTERS:
		{
			RECT rc;
			HWND letters_text = (HWND) GetWindowLongPtr(window, INDEX_LETTERS);
			HBRUSH bg_brush = CreateSolidBrush(BGCOLOR);
			HDC dc = GetDC(letters_text);
			char *str = (char *) lparam;
			GetClientRect(letters_text, &rc);
			FillRect(dc, &rc, bg_brush);
			SetWindowText(letters_text, str);
			ReleaseDC(letters_text, dc);
			DeleteObject(bg_brush);
		} break;
	case WM_CREATE:
		{
			WINDOWSIZE wsize = GetWindowSize(window);
			CREATESTRUCT cs = *((CREATESTRUCT *) lparam);

			HWND input_text = CreateWindow(
										   "STATIC",
										   "",
										   (WS_VISIBLE | WS_CHILD | SS_CENTER | SS_SUNKEN | SS_ENDELLIPSIS | SS_CENTERIMAGE),
										   0, 0, wsize.width, wsize.height / 2,
										   window,
										   0,
										   cs.hInstance,
										   0);
			HWND letters_text = CreateWindow(
											 "STATIC",
											 "",
											 (WS_VISIBLE | WS_CHILD | SS_CENTER | SS_ENDELLIPSIS | SS_CENTERIMAGE),
											 0, wsize.height / 2, wsize.width, wsize.height / 2,
											 window,
											 0,
											 cs.hInstance,
											 0);

			HDC dc = GetDC(window);
			HFONT font = CreateFontA(
									 -MulDiv(16, GetDeviceCaps(dc, LOGPIXELSY), 72),
									 -MulDiv(16, GetDeviceCaps(dc, LOGPIXELSY), 72),
									 0, 0, FW_BLACK, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
									 OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
									 DEFAULT_PITCH, "Verdana\0");

			ReleaseDC(window, dc);
			SendMessage(input_text, WM_SETFONT, (WPARAM) font, TRUE);
			SendMessage(letters_text, WM_SETFONT, (WPARAM) font, TRUE);
			
			SetWindowLongPtr(window, INDEX_INPUT, (LONG_PTR) input_text);
			SetWindowLongPtr(window, INDEX_LETTERS, (LONG_PTR) letters_text);
		} break;
	}

	return DefWindowProc(window, msg, wparam, lparam);
}

BOOL InputMakeWindow(HWND *hwnd, HWND parent, HINSTANCE instance)
{
	UINT height;
	UINT y_offset;
	WINDOWSIZE size;
	WNDCLASS wc = {0};
	HBRUSH bg_brush = CreateSolidBrush(BGCOLOR);

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_NOCLOSE;
	wc.lpfnWndProc = InputWindowProc;
	wc.hInstance = instance;
	wc.hbrBackground = bg_brush;
	wc.lpszClassName = CLASS_NAME_INPUT;
	wc.cbWndExtra = sizeof(LONG_PTR) * 2;
	wc.hCursor = LoadCursor(NULL, (LPCTSTR)IDC_ARROW);
	
	if (!RegisterClass(&wc)) {
		return FALSE;
	}

	// TODO: Clean this up!
	size = GetWindowSize(parent);
	y_offset = size.height * (RATIO_ACTION + RATIO_STATUS + RATIO_INPUT);
	height = size.height * RATIO_INPUT;

	*hwnd = CreateWindow(
						 CLASS_NAME_INPUT,
						 TITLE_INPUT,
						 (WS_CHILD | WS_VISIBLE),
						 0, (size.height - y_offset), size.width, height,
						 parent, 0,
						 instance,
						 0);

	if (!*hwnd) {
		*hwnd = NULL;
		return FALSE;
	}

	return TRUE;
}
