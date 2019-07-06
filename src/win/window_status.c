#include <windows.h>

LRESULT CALLBACK StatusWindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(window, msg, wparam, lparam);
}

BOOL StatusMakeWindow(HWND *action, HINSTANCE instance)
{
	WNDCLASS wc = {0};
	HBRUSH bg_brush = CreateSolidBrush(BGCOLOR);

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = StatusWindowProc;
	wc.hInstance = instance;
	wc.hbrBackground = bg_brush;
	wc.lpszClassName = CLASS_NAME_STATUS;

	if (!RegisterClass(&wc)) {
		return FALSE;
	}
	
	*action = CreateWindow(
						CLASS_NAME_STATUS,
						TITLE_STATUS,
						WS_OVERLAPPEDWINDOW,
						CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
						0, 0,
						instance,
						0);
	if (!*action) {
		*action = NULL;
		return FALSE;
	}

	return TRUE;
}
