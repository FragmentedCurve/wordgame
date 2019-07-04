#include <windows.h>
#include <stdbool.h>
#include <common.h>
#include "mm.h"

#define INPUT_WIDTH 200
#define INPUT_HEIGHT 100

static LRESULT CALLBACK InputWindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
	case WM_KEYUP:
		{
			char c = wparam;
			if (c == VK_RETURN) {
				// TODO: Check word against the current game
				DEBUG("Enter Pressed");
			} else if (c >= 'A' && c <= 'Z') {
				DEBUG("Letter pressed");
				// TODO: Add the letter to the buffer.
			}
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
