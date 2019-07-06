#ifdef _WIN32
#define DEBUG(msg) MessageBox(NULL, TEXT(msg), "DEBUG", 0)
#else
#define DEBUG(msg) fprintf(stderr, "DEBUG: %s\n", msg)
#endif

#include <stdio.h>
#include <stdbool.h>

#define ALPHABET_SIZE 26 // TODO: Make the alphabet variable to support any kind of wordlist.

#define internal static  // internal static functions
#define global static    // global variables
#define persist static   // local persistent static variables

typedef struct TrieNode TrieNode;
struct TrieNode {
	TrieNode *children[ALPHABET_SIZE];
	int is_word;
};

/* TODO: Make sure all these error states are being used somewhere. */
typedef enum GameError GameError;
enum GameError {
				NONE = 0,

				// Letter
				NOT_ALPHA,
				WRONG_LETTER,
				
				// Word
				WORD_PLAYED,
				INCORRECT_WORD,
				INPUT_BUF_FULL,
				INPUT_BUF_EMPTY,
				
				// Game
				GAME_LOST,
				GAME_WON,
};

typedef struct Game Game;
struct Game {
	TrieNode *words;         // Dictionary words
	TrieNode *played_words;  // Words successfully played
	char *letters;           // Random letters for player
	unsigned int word_size;  // Max size a word can be
	int repeat_max;          // Max times a letter can repeat
	char *play_buffer;       // Used for storing player input
};

/* trie.c */
TrieNode *new_trienode();
void free_trienode(TrieNode *root);
void insert_word(TrieNode *trie, const char *word);
bool word_exists(TrieNode *trie, const char *word);

/* game.c */
Game new_game(const char* const word_list[], unsigned int wl_len, unsigned int word_size, int repeat_max);
void destroy_game(Game game);
void reset_game(Game *game, unsigned int word_size);
void shuffle(Game game);
GameError check(Game game);
GameError check_word(Game game, const char *word);
GameError play(Game game);
GameError play_word(Game game, char *word);
GameError input_char(Game game, char c);
GameError delete_char(Game game);
unsigned int *played_word_count(Game game);
unsigned int *possible_word_count(Game game);

/* words.c */
bool is_word_alpha(const char *word);
bool in_word(char c, const char *word);
int in_word_count(char c, const char *word);
bool is_similar(const char *word1, const char *word2);
char *rand_letters(int len, int repeat_max);
