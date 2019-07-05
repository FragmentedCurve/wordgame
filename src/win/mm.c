#include <windows.h>
#include <stdio.h>
#include <stdbool.h>

#include "common.h"
#include "mm.h"
#include "dict.h"

#define MAIN_WIDTH  1000
#define MAIN_HEIGHT 600

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

/* TODO: Find a way of preventing these from being global. */
global HWND main;
global HWND action;
global HWND input;

global Game game = {0};

WINDOWSIZE GetWindowSize(HWND window)
{
	RECT rc = {0};
	WINDOWSIZE size = {0};
	GetClientRect(window, &rc);
	size.width = rc.right - rc.left;
	size.height = rc.bottom - rc.top;
	return size;
}

void player_input(Game game, char c)
{
	GameError result = NONE;
	
	if (c >= 'A' && c <= 'Z') {
		result = input_char(game, c);
	} else if (c == VK_BACK) {
		result = delete_char(game);
	} else if (c == VK_RETURN) {
		result = check(game);
	}

	// TODO: Handle errors correctly
	switch (result) {
	case NONE:
		{
			PostMessage(input, WM_PLAYERINPUT, 0, (LPARAM) game.play_buffer);
		} break;
	case INPUT_FULL:
		{
			DEBUG("Input Full!");
		} break;
	default:
		{
			DEBUG("Something else");
		} break;
	}
}

LRESULT CALLBACK MainWindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
	case WM_KEYUP:

		{
			char c = wparam;
			player_input(game, c);
		} break;
	case WM_NEWGAME:
		{
			reset_game(&game, WORD_SIZE);
			PostMessage(input, WM_PLAYERINPUT, 0, (LPARAM) game.play_buffer);
			PostMessage(input, WM_GAMELETTERS, 0, (LPARAM) game.letters);
		} break;
	case WM_SHUFFLE:
		{
			shuffle(game);
			PostMessage(input, WM_GAMELETTERS, 0, (LPARAM) game.letters);
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
	
	result = MainMakeWindow(&main, MAIN_WIDTH, MAIN_HEIGHT, instance);
	if (!result)
		return 0;

	result = ActionMakeWindow(&action, main, instance);
	if (!result)
		return 0;

	result = InputMakeWindow(&input, main, instance);
	if (!result)
		return 0;

	ShowWindow(main, cmd_show);
	game = new_game(wl_common6, wl_common6_len, WORD_SIZE, WORD_SIZE);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return 0;
}
