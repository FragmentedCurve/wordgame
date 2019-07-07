#include <windows.h>

#include "mm.h"

LRESULT CALLBACK PlayboardWindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
	case WM_SIZE:
		{
			HWND parent = GetParent(window);
			WINDOWSIZE size = GetWindowSize(parent);

			size.height *= 1 - (RATIO_ACTION + RATIO_STATUS + RATIO_INPUT);
			MoveWindow(window, 0, 0, size.width, size.height, TRUE);
		} break;
	}
	
	return DefWindowProc(window, msg, wparam, lparam);
}

BOOL PlayboardMakeWindow(HWND *playboard, HWND parent, HINSTANCE instance)
{
	WNDCLASS wc = {0};
	HBRUSH bg_brush = CreateSolidBrush(RGB(0xFF, 0, 0)); // CreateSolidBrush(BGCOLOR);

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = PlayboardWindowProc;
	wc.hInstance = instance;
	wc.hbrBackground = bg_brush;
	wc.lpszClassName = CLASS_NAME_PLAYBOARD;
		
	if (!RegisterClass(&wc)) {
		return FALSE;
	}

	WINDOWSIZE size = GetWindowSize(parent);
	size.height -= size.height * (RATIO_ACTION + RATIO_STATUS + RATIO_INPUT);
	
	*playboard = CreateWindow(
						CLASS_NAME_PLAYBOARD,
						TITLE_PLAYBOARD,
						(WS_CHILD | WS_VISIBLE),
						0, 0, size.width, size.height,
						parent,
						0,
						instance,
						0);
	
	if (!*playboard) {
		*playboard = NULL;
		return FALSE;
	}

	return TRUE;
}
