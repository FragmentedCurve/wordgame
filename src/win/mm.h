#define RED     RGB(0xFF, 0x00, 0x00)
#define GREEN   RGB(0x00, 0xFF, 0x00)
#define BLUE    RGB(0x00, 0x00, 0xFF)
#define BGCOLOR RGB(0xF0, 0xF0, 0xF0)

#define WORD_SIZE 6

// TODO: These ratio definitions might be obsolete
#define RATIO_ACTION 1.0/10.0
#define RATIO_INPUT  3.0/10.0
#define RATIO_STATUS 1.0/10.0

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

typedef struct GAMESTATE {
	char play_buffer[WORD_SIZE];
	char available[ALPHABET_SIZE];
} GAMESTATE;

/* mm.c */
WINDOWSIZE GetWindowSize(HWND window);

/* window_action.c */
BOOL ActionMakeWindow(HWND *hwnd, HWND parent, HINSTANCE instance);

/* window_input.c */
BOOL InputMakeWindow(HWND *hwnd, HWND parent, HINSTANCE instance);
