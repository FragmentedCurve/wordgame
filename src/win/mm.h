#define RED     RGB(0xFF, 0x00, 0x00)
#define GREEN   RGB(0x00, 0xFF, 0x00)
#define BLUE    RGB(0x00, 0x00, 0xFF)
#define BGCOLOR RGB(0xF0, 0xF0, 0xF0)

#define WORD_SIZE 6

// TODO: These ratio definitions might be obsolete
#define RATIO_ACTION 1.0/10.0
#define RATIO_INPUT  2.0/10.0
#define RATIO_STATUS 1.0/10.0

/* WINAPI Messages */
#define WM_PLAYERINPUT   (WM_USER + 1)   // LPARAM = pointer to string to display.
#define WM_GAMELETTERS   (WM_USER + 2)   // Lparam = pointer to string to display.
#define WM_NEWGAME       (WM_USER + 3)
#define WM_SHUFFLE       (WM_USER + 4)

/* WNDCLASS LONG_PTR Indexes */
#define INDEX_GAME    0
#define INDEX_WINPUT  1 * sizeof(LONG_PTR)
#define INDEX_WACTION 2 * sizeof(LONG_PTR)

extern const char *CLASS_NAME_MAIN;
extern const char *CLASS_NAME_ACTION;
extern const char *CLASS_NAME_STATUS;   
extern const char *CLASS_NAME_INPUT;   
extern const char *CLASS_NAME_PLAYBOARD;

extern const char *TITLE_MAIN;
extern const char *TITLE_ACTION;
extern const char *TITLE_STATUS;
extern const char *TITLE_INPUT;
extern const char *TITLE_PLAYBOARD;

typedef struct WINDOWSIZE {
	UINT width;
	UINT height;
} WINDOWSIZE;

/* mm.c */
WINDOWSIZE GetWindowSize(HWND window);

/* window_action.c */
BOOL ActionMakeWindow(HWND *hwnd, HWND parent, HINSTANCE instance);

/* window_input.c */
BOOL InputMakeWindow(HWND *hwnd, HWND parent, HINSTANCE instance);
