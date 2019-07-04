#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <common.h>
#include "mm.h"

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

GAMESTATE game_state = {0};

internal HWND main;
internal HWND action;
internal HWND input;

WINDOWSIZE GetWindowSize(HWND window)
{
	RECT rc = {0};
	WINDOWSIZE size = {0};
	GetClientRect(window, &rc);
	size.width = rc.right - rc.left;
	size.height = rc.bottom - rc.top;
	return size;
}

LRESULT CALLBACK PlayboardWindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(window, msg, wparam, lparam);
}

LRESULT CALLBACK StatusWindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(window, msg, wparam, lparam);
}

LRESULT CALLBACK MainWindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
	case WM_KEYUP:
		{
			char c = wparam;
			if (c == VK_RETURN || (c >= 'A' && c <= 'Z'))
				PostMessage(input, msg, wparam, lparam);
		} break;
	case WM_DESTROY:
		{
			PostQuitMessage(0);
		} break;
	}
	
	return DefWindowProc(window, msg, wparam, lparam);
}

BOOL PlayboardMakeWindow(HWND *action, HINSTANCE instance)
{
	WNDCLASS wc = {0};
	HBRUSH bg_brush = CreateSolidBrush(BGCOLOR);

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = PlayboardWindowProc;
	wc.hInstance = instance;
	wc.hbrBackground = bg_brush;
	wc.lpszClassName = CLASS_NAME_PLAYBOARD;

	if (!RegisterClass(&wc)) {
		return FALSE;
	}
	
	*action = CreateWindow(
						CLASS_NAME_PLAYBOARD,
						TITLE_PLAYBOARD,
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
	/*
	FILE *dict = fopen("resources/en-common.wl", "r");
	Game game = new_game(dict, WORD_SIZE);
	fclose(dict);
	*/
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return 0;
}
