#include <windows.h>
#include <stdbool.h>
#include <common.h>
#include "mm.h"

struct ActionButtons {
	HWND new_game;
	HWND shuffle;
};

internal WINDOWSIZE GetButtonSize(HWND w)
{
	WINDOWSIZE bsize = {0};
	WINDOWSIZE wsize = GetWindowSize(w);

	bsize.width = wsize.width / 2;
	bsize.height = wsize.height;
	
	return bsize;
}

static LRESULT CALLBACK ActionWindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
	case WM_COMMAND:
		{
			struct ActionButtons *buttons = (struct ActionButtons *) GetWindowLongPtr(window, 0);
			HWND b = (HWND) lparam;
			
			if (b == buttons->new_game) {
				HWND parent = GetParent(window);
				PostMessage(parent, WM_NEWGAME, 0, 0);
			} else if (b == buttons->shuffle) {
				HWND parent = GetParent(window);
				PostMessage(parent, WM_SHUFFLE, 0, 0);
			} else {
				PostQuitMessage(0);
			}
		} break;
	case WM_SIZE:
		{
			HWND parent = GetParent(window);
			WINDOWSIZE wsize = GetWindowSize(parent);
			UINT y_offset = wsize.height * (1 - RATIO_ACTION);
			wsize.height *= RATIO_ACTION;

			MoveWindow(window, 0, y_offset, wsize.width, wsize.height, FALSE);

			WINDOWSIZE bsize = GetButtonSize(window);
			struct ActionButtons *buttons = (struct ActionButtons *) GetWindowLongPtr(window, 0);
			
			SetWindowPos(
						 buttons->new_game,
						 NULL,
						 0, 0, bsize.width, bsize.height,
						 (SWP_NOZORDER | SWP_SHOWWINDOW));
			SetWindowPos(
						 buttons->shuffle,
						 NULL,
						 bsize.width, 0, bsize.width, bsize.height,
						 (SWP_NOZORDER | SWP_SHOWWINDOW));
		} break;
	case WM_CREATE:
		{
			WINDOWSIZE bsize = GetButtonSize(window);
			CREATESTRUCT cs = *((CREATESTRUCT *) lparam);
			// Warning: If for some reason in the future this window gets destroyed
			// and recreated throughout the run-life of the application, this will leak memory.
			struct ActionButtons *buttons = calloc(1, sizeof(struct ActionButtons));
			
			buttons->new_game = CreateWindow("BUTTON", "New Game",
											 (BS_DEFPUSHBUTTON | BS_FLAT | WS_VISIBLE | WS_CHILD),
											 0, 0, bsize.width,  bsize.height,
											 window,
											 0,
											 cs.hInstance,
											 0);

			buttons->shuffle = CreateWindow("BUTTON", "Shuffle",
										   (BS_DEFPUSHBUTTON | BS_FLAT | WS_VISIBLE | WS_CHILD),
										   bsize.width, 0, bsize.width,  bsize.height,
										   window,
										   0,
										   cs.hInstance,
										   0);
			SetWindowLongPtr(window, 0, (LONG_PTR) buttons);
		} break;
	}

	return DefWindowProc(window, msg, wparam, lparam);
}

BOOL ActionMakeWindow(HWND *hwnd, HWND parent, HINSTANCE instance)
{
	UINT width, height;
	WINDOWSIZE size;
	WNDCLASS wc = {0};
	HBRUSH bg_brush = CreateSolidBrush(BGCOLOR);

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_NOCLOSE;
	wc.lpfnWndProc = ActionWindowProc;
	wc.hInstance = instance;
	wc.hbrBackground = bg_brush;
	wc.lpszClassName = CLASS_NAME_ACTION;
	wc.cbWndExtra = sizeof(LONG_PTR);

	if (!RegisterClass(&wc)) {
		return FALSE;
	}

	size = GetWindowSize(parent);
	width = size.width;
	height = size.height * RATIO_ACTION;
	
	*hwnd = CreateWindow(
						CLASS_NAME_ACTION,
						TITLE_ACTION,
						(WS_CHILD | WS_VISIBLE),
						0, (size.height - height), width, height,
						parent, 0,
						instance,
						0);
	if (!*hwnd) {
		*hwnd = NULL;
		return FALSE;
	}

	return TRUE;
}
