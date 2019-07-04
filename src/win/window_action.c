#include <windows.h>
#include <stdbool.h>
#include <common.h>
#include "mm.h"

#define ACTION_WIDTH 250
#define ACTION_HEIGHT 100

#define B_CMD_NEWGAME 1
#define B_CMD_SHUFFLE 2
#define B_CMD_QUIT    3

struct ActionButtons {
	HWND new_game;
	HWND shuffle;
	HWND quit;
};

internal const char *B_TITLE_NEWGAME = "New Game";
internal const char *B_TITLE_SHUFFLE = "Shuffle";
internal const char *B_TITLE_QUIT    = "Quit";
internal struct ActionButtons buttons = {0};

static WINDOWSIZE GetButtonSize(HWND w)
{
	WINDOWSIZE bsize = {0};
	WINDOWSIZE wsize = GetWindowSize(w);

	bsize.width = wsize.width / 3;
	bsize.height = wsize.height;
	
	return bsize;
}

static LRESULT CALLBACK ActionWindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
	case WM_COMMAND:
		{
			UINT cmd = LOWORD(wparam);

			switch (cmd) {
			case B_CMD_NEWGAME:
				{
					DEBUG("New game!");
				} break;
			case B_CMD_SHUFFLE:
				{
					DEBUG("Do the Shuffle!");
				} break;
			case B_CMD_QUIT:
				{
					PostQuitMessage(0);
				} break;
			}
		} break;
	case WM_SIZE:
		{
			WINDOWSIZE bsize = GetButtonSize(window);
			WINDOWSIZE wsize = GetWindowSize(window);

			SetWindowPos(
						 buttons.new_game,
						 NULL,
						 0, 0, bsize.width, bsize.height,
						 (SWP_NOZORDER | SWP_SHOWWINDOW));
			SetWindowPos(
						 buttons.shuffle,
						 NULL,
						 bsize.width, 0, bsize.width, bsize.height,
						 (SWP_NOZORDER | SWP_SHOWWINDOW));
			SetWindowPos(
						 buttons.quit,
						 NULL,
						 (2 * bsize.width), 0, (bsize.width + (wsize.width - 3 * bsize.width)), bsize.height,
						 (SWP_NOZORDER | SWP_SHOWWINDOW));
		} break;
	case WM_CREATE:
		{
			WINDOWSIZE bsize = GetButtonSize(window);
			WINDOWSIZE wsize = GetWindowSize(window);
			CREATESTRUCT cs = *((CREATESTRUCT *) lparam);
			
			buttons.new_game = CreateWindow("BUTTON", "New Game",
											(BS_DEFPUSHBUTTON | BS_FLAT | WS_VISIBLE | WS_CHILD),
											0, 0, bsize.width,  bsize.height,
											window,
											(HMENU) B_CMD_NEWGAME,
											cs.hInstance,
											0);

			buttons.shuffle = CreateWindow("BUTTON", "Shuffle",
										   (BS_DEFPUSHBUTTON | BS_FLAT | WS_VISIBLE | WS_CHILD),
										   bsize.width, 0, bsize.width,  bsize.height,
										   window,
										   (HMENU) B_CMD_SHUFFLE,
										   cs.hInstance,
										   0);

			buttons.quit = CreateWindow("BUTTON", "Quit",
										(BS_DEFPUSHBUTTON | BS_FLAT | WS_VISIBLE | WS_CHILD),
										(2 * bsize.width), 0, bsize.width + (wsize.width - 3 * bsize.width),  bsize.height,
										window,
										(HMENU) B_CMD_QUIT,
										cs.hInstance,
										0);
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

	if (!RegisterClass(&wc)) {
		return FALSE;
	}

	size = GetWindowSize(parent);
	width = size.width;
	height = size.height * RATIO_ACTION;
	
	*hwnd = CreateWindow(
						CLASS_NAME_ACTION,
						TITLE_ACTION,
						(WS_SIZEBOX | WS_CAPTION | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS),
						CW_USEDEFAULT, CW_USEDEFAULT, ACTION_WIDTH, ACTION_HEIGHT,
						parent, 0,
						instance,
						0);
	if (!*hwnd) {
		*hwnd = NULL;
		return FALSE;
	}

	return TRUE;
}
