#include <windows.h>
#include <stdbool.h>

#include "common.h"
#include "mm.h"

#define INPUT_WIDTH 200
#define INPUT_HEIGHT 100

global const char *S_TITLE_INPUTTEXT = "InputText";

global HWND input_text;
global HWND letters_text;

static LRESULT CALLBACK InputWindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
	case WM_SIZE:
		{
			WINDOWSIZE wsize = GetWindowSize(window);
			SetWindowPos(
						 input_text,
						 NULL,
						 0, 0, wsize.width, wsize.height / 2,
						 (SWP_NOZORDER | SWP_SHOWWINDOW));
			SetWindowPos(
						 letters_text,
						 NULL,
						 0, wsize.height / 2, wsize.width, wsize.height / 2,
						 (SWP_NOZORDER | SWP_SHOWWINDOW));
						
		} break;
	case WM_PLAYERINPUT:
		{
			RECT rc;
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
			HBRUSH bg_brush = CreateSolidBrush(BGCOLOR); // TODO: Does this have to be freed?
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
			
			input_text = CreateWindow(
									  "STATIC",
									  "",
									  WS_VISIBLE | WS_CHILD | SS_SUNKEN | SS_SIMPLE,
									  0, 0, wsize.width, wsize.height / 2,
									  window,
									  0,
									  cs.hInstance,
									  0);
			letters_text = CreateWindow(
									  "STATIC",
									  "",
									  (WS_VISIBLE | WS_CHILD | SS_CENTER | SS_SUNKEN | SS_ENDELLIPSIS),
									  0, wsize.height / 2, wsize.width, wsize.height / 2,
									  window,
									  0,
									  cs.hInstance,
									  0);

		} break;
	}

	return DefWindowProc(window, msg, wparam, lparam);
}

BOOL InputMakeWindow(HWND *hwnd, HWND parent, HINSTANCE instance)
{
	UINT height;
	UINT y_stack;
	WINDOWSIZE size;
	WNDCLASS wc = {0};
	HBRUSH bg_brush = CreateSolidBrush(BGCOLOR);

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_NOCLOSE;
	wc.lpfnWndProc = InputWindowProc;
	wc.hInstance = instance;
	wc.hbrBackground = bg_brush;
	wc.lpszClassName = CLASS_NAME_INPUT;

	if (!RegisterClass(&wc)) {
		return FALSE;
	}

	size = GetWindowSize(parent);
	y_stack = size.height * (RATIO_ACTION + RATIO_STATUS + RATIO_INPUT);
	height = size.height * RATIO_INPUT;

	*hwnd = CreateWindow(
						 CLASS_NAME_INPUT,
						 TITLE_INPUT,
						 (WS_SIZEBOX | WS_CAPTION | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS),
						 CW_USEDEFAULT, CW_USEDEFAULT, INPUT_WIDTH, INPUT_HEIGHT,
						 parent, 0,
						 instance,
						 0);

	if (!*hwnd) {
		*hwnd = NULL;
		return FALSE;
	}

	return TRUE;
}
