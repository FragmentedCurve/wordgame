#include <windows.h>
#include <stdio.h>
#include <stdbool.h>

#include "common.h"
#include "mm.h"
#include "dict.h"

#define MAIN_WIDTH  300
#define MAIN_HEIGHT 600

/* WNDCLASS LONG_PTR Indexes */
#define INDEX_GAME    0
#define INDEX_WINPUT  1 * sizeof(LONG_PTR)
#define INDEX_WACTION 2 * sizeof(LONG_PTR)

const char *CLASS_NAME_MAIN      = "Main";
const char *CLASS_NAME_ACTION    = "Action";
const char *CLASS_NAME_STATUS    = "Status";
const char *CLASS_NAME_INPUT     = "Input";
const char *CLASS_NAME_PLAYBOARD = "Playboard";

const char *TITLE_MAIN      = "Main";
const char *TITLE_ACTION    = "Action";
const char *TITLE_STATUS    = "Status";
const char *TITLE_INPUT     = "Input";
const char *TITLE_PLAYBOARD = "Playboard";

WINDOWSIZE GetWindowSize(HWND window)
{
	RECT rc = {0};
	WINDOWSIZE size = {0};
	GetClientRect(window, &rc);
	size.width = rc.right - rc.left;
	size.height = rc.bottom - rc.top;
	return size;
}

internal void player_input(Game game, HWND input, char c)
{
	GameError result = NONE;
	
	if (c >= 'A' && c <= 'Z') {
		result = input_char(game, c);
	} else if (c == VK_BACK) {
		result = delete_char(game);
	} else if (c == VK_RETURN) {
		result = play(game);
	}

	switch (result) {
	case NONE:
		{
			PostMessage(input, WM_PLAYERINPUT, 0, (LPARAM) game.play_buffer);
		} break;
	case INPUT_BUF_FULL:
		{
			//DEBUG("Too many letters!");
		} break;
	case INPUT_BUF_EMPTY:
		{
			//DEBUG("No letters to delete.");
		} break;
	case WORD_PLAYED:
		{
			DEBUG("You already played that word!");
		} break;
	case WRONG_LETTER:
		{
			//DEBUG("That's not one of the given random letters!");
		} break;
	case INCORRECT_WORD:
		{
			DEBUG("That's not even a word!");
		} break;
	default:
		{
			DEBUG("I don't know what happened. Maybe you do.");
		} break;
	}
}

LRESULT CALLBACK MainWindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
	Game *game = (Game *) GetWindowLongPtr(window, INDEX_GAME);
	HWND input = (HWND) GetWindowLongPtr(window, INDEX_WINPUT);
	HWND action = (HWND) GetWindowLongPtr(window, INDEX_WACTION);

	switch (msg) {
	case WM_CREATE:
		{
			CREATESTRUCT cs = *((CREATESTRUCT *) lparam);
			
			ActionMakeWindow(&action, window, cs.hInstance);
			InputMakeWindow(&input, window, cs.hInstance);

			SetWindowLongPtr(window, INDEX_WINPUT, (LONG_PTR) input);
			SetWindowLongPtr(window, INDEX_WACTION, (LONG_PTR) action);
		} break;
	case WM_SIZE:
		{
			SendMessage(action, msg, wparam, lparam);
			SendMessage(input, msg, wparam, lparam);
		} break;
	case WM_KEYDOWN:
		{
			char c = wparam;
			player_input(*game, input, c);
		} break;
	case WM_NEWGAME:
		{
			reset_game(game, WORD_SIZE);
			SendMessage(input, WM_PLAYERINPUT, 0, (LPARAM) game->play_buffer);
			SendMessage(input, WM_GAMELETTERS, 0, (LPARAM) game->letters);
			SetFocus(window);
		} break;
	case WM_SHUFFLE:
		{
			shuffle(*game);
			PostMessage(input, WM_GAMELETTERS, 0, (LPARAM) game->letters);
			SetFocus(window);
			break;
		} break;
	case WM_DESTROY:
		{
			PostQuitMessage(0);
		} break;
	}
	
	return DefWindowProc(window, msg, wparam, lparam);
}

BOOL MainMakeWindow(HWND *hwnd, UINT width, UINT height, HINSTANCE instance)
{
	WNDCLASS wc = {0};
	HBRUSH bg_brush = CreateSolidBrush(BGCOLOR);

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWindowProc;
	wc.hInstance = instance;
	wc.hbrBackground = bg_brush;
	wc.lpszClassName = CLASS_NAME_MAIN;
	wc.cbWndExtra = sizeof(LONG_PTR) * 4;
	
	if (!RegisterClass(&wc))
		return FALSE;

	*hwnd = CreateWindow(
						CLASS_NAME_MAIN,
						TITLE_MAIN,
						WS_OVERLAPPEDWINDOW,
						CW_USEDEFAULT, CW_USEDEFAULT, width, height,
						0, 0,
						instance,
						0);
	if (!*hwnd) {
		*hwnd = NULL;
		return FALSE;
	}

	return TRUE;
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR cmd_line, int cmd_show)
{
	BOOL result;
	HWND main;
	HWND input;
	Game game = {0};

	result = MainMakeWindow(&main, MAIN_WIDTH, MAIN_HEIGHT, instance);
	if (!result)
		return 0;

	ShowWindow(main, cmd_show);

	// Doing this in WM_CREATE throws an "Exception 0xc0000005".
	// Don't know why. Possibly due to recusion
	game = new_game(wl_common6, wl_common6_len, WORD_SIZE, WORD_SIZE);
	SetWindowLongPtr(main, INDEX_GAME, (LONG_PTR) &game);
	input = (HWND) GetWindowLongPtr(main, INDEX_WINPUT);
	SendMessage(input, WM_GAMELETTERS, 0, (LPARAM) game.letters);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return 0;
}
