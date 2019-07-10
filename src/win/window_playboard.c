#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "mm.h"
#include "common.h"

#define INDEX_BOARD 0
#define INDEX_MARGIN sizeof(LONG_PTR)

LRESULT CALLBACK PlayboardWindowProc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg) {
	case WM_UPDATE_BOARD:
		{
			// TODO: Redo all of this. This heavily depends on WORD_SIZE being 6.
			// TODO: THIS IS ALL SHIT.
			HWND board = (HWND) GetWindowLongPtr(window, INDEX_BOARD);
			char *lines[WORD_SIZE];
			const char *fmt = "%d letters: %2d/%2d\n";
			int *counts = (int *) wparam;
			int *maxes = (int *) lparam;

			int total_len = 0;
			for (int i = 0; i < WORD_SIZE; i++) {
				// TODO: This is lazy. Do this better.
				int max_len = strlen(fmt) * 2;
				lines[i] = calloc(max_len, sizeof(char));
				total_len += snprintf(lines[i], max_len, fmt, (i + 1), counts[i], maxes[i]);
			}

			int len = total_len + WORD_SIZE + 1;
			char *final = calloc(total_len + WORD_SIZE + 1, sizeof(char));
			for (int i = 0; i < WORD_SIZE; i++) {
				int left = len - strlen(final);
				strncpy_s(&final[strlen(final)], left, lines[i], strlen(lines[i]));
				final[strlen(final)] = '\n';
			}
			final[strlen(final)] = '\0';
			SetWindowText(board, final);

			for (int i = 0; i < WORD_SIZE; i++) {
				free(lines[i]);
			}
			free(final);
		} break;
	case WM_SIZE:
		{
			
			HWND parent = GetParent(window);
			HWND board = (HWND) GetWindowLongPtr(window, INDEX_BOARD);
			RECT *rc = (RECT *) GetWindowLongPtr(window, INDEX_MARGIN);
			WINDOWSIZE size = GetWindowSize(parent);

			size.height *= 1 - (RATIO_ACTION + RATIO_STATUS + RATIO_INPUT);
			MoveWindow(window, 0, 0, size.width, size.height, TRUE);
			MoveWindow(
					   board,
					   rc->left, rc->top,
					   size.width - (rc->left + rc->right),
					   size.height - (rc->top + rc->bottom),
					   TRUE);
		} break;
	case WM_CREATE:
		{
			CREATESTRUCT cs = *((CREATESTRUCT *) lparam);
			WINDOWSIZE size = GetWindowSize(window);
			RECT *rc = calloc(1, sizeof(RECT));
			assert(rc);
			*rc =  (RECT) {
					   .top = 25,
					   .left = 25,
					   .right = 25,
					   .bottom = 25
			};

			HWND board = CreateWindow(
									  "STATIC",
									  "",
									  (WS_CHILD | WS_VISIBLE | SS_CENTER),
									  rc->left, rc->top,
									  size.width - (rc->left + rc->right),
									  size.height - (rc->bottom + rc->top),
									  window,
									  0,
									  cs.hInstance,
									  0);

			HDC dc = GetDC(board);
			HFONT font = CreateFontA(
									 -MulDiv(8, GetDeviceCaps(dc, LOGPIXELSY), 72),
									 -MulDiv(8, GetDeviceCaps(dc, LOGPIXELSY), 72),
									 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
									 OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
									 DEFAULT_PITCH, "Verdana\0");
			SendMessage(board, WM_SETFONT, (WPARAM) font, TRUE);
			ReleaseDC(board, dc);
			
			SetWindowLongPtr(window, INDEX_BOARD, (LONG_PTR) board);
			SetWindowLongPtr(window, INDEX_MARGIN, (LONG_PTR) rc);
		} break;
	}
	
	return DefWindowProc(window, msg, wparam, lparam);
}

BOOL PlayboardMakeWindow(HWND *playboard, HWND parent, HINSTANCE instance)
{
	WNDCLASS wc = {0};
	HBRUSH bg_brush = CreateSolidBrush(BGCOLOR);

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = PlayboardWindowProc;
	wc.hInstance = instance;
	wc.hbrBackground = bg_brush;
	wc.lpszClassName = CLASS_NAME_PLAYBOARD;
	wc.cbWndExtra = sizeof(LONG_PTR) + sizeof(RECT);
	wc.hCursor = LoadCursor(NULL, (LPCTSTR)IDC_ARROW);
	
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
