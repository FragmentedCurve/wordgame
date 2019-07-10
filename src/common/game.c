#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>
#include <common.h>

internal char *remove_letter(const char *s, int index)
{
	int len = strlen(s);
	char *w = calloc(len, sizeof(char));
	char *p = w;
	
	for (int i = 0; i < len; i++) {
		if (i != index) {
			*p = s[i];
			p++;
		}
	}

	return w;
}

Game new_game(const char* const word_list[], unsigned int wl_len, unsigned int word_size, int repeat_max)
{
	Game game = {0};

	game.word_size = word_size;
	game.repeat_max = repeat_max;
	game.words = new_trienode();
	game.played_words = new_trienode();
	game.letters = rand_letters(game.word_size, game.repeat_max);
	game.repeat_max = repeat_max;
	game.play_buffer = calloc(game.word_size + 1, sizeof(char));
	assert(game.play_buffer);
	
	for (int i = 0; i < wl_len; i++)
		if (is_word_alpha(word_list[i]) && strlen(word_list[i]) <= game.word_size)
			insert_word(game.words, word_list[i]);
	
	return game;
}

void destroy_game(Game game)
{
	free_trienode(game.words);
	free_trienode(game.played_words);
	free(game.letters);
	free(game.play_buffer);
}

GameError play(Game game)
{
	return play_word(game, game.play_buffer);
}

/*
  Checks if word is valid and adds it to the played words trie (game.played_words).
  Its similar to check_word() except play_word() changes the game state by adding
  the word into the trie.
*/
GameError play_word(Game game, char *word)
{
	GameError result = check_word(game, word);
	
	if (result == NONE) {
		insert_word(game.played_words, word);
		clear_buffer(game);
	}
	
	return result;
}

/*
  Resetting the game implies NOT changing the word list used to make 
  the game. The only things reset are the word size, the player's buffer,
  and the playable random letters.
*/
void reset_game(Game *game, unsigned int word_size)
{
	free_trienode(game->played_words);
	free(game->letters);
	game->word_size = word_size;
	game->played_words = new_trienode();
	game->letters = rand_letters(game->word_size, game->repeat_max);
	clear_buffer(*game);
}

/*
  Shuffle the player's given random letters (game.letters).
*/
void shuffle(Game game)
{
	int len = strlen(game.letters);

	while (*game.letters) {
		int i = rand() % len;
		char c = *game.letters;
		*game.letters = game.letters[i];
		game.letters[i] = c;
		game.letters++;
		len--;
	}
}

/*
  Appends a char to the player's buffer (game.play_buffer).
*/
GameError input_char(Game game, char c)
{
	int n = strlen(game.play_buffer);
	
	if (n == game.word_size)
		return INPUT_BUF_FULL;

	game.play_buffer[n] = c;

	// Make sure the char is a given random letter.
	if (!is_similar(game.play_buffer, game.letters)) {
		game.play_buffer[n] = '\0';
		return WRONG_LETTER;
	}

	return NONE;
}

/*
  Removes the last char in game.play_buffer.
*/
GameError delete_char(Game game)
{
	int n = strlen(game.play_buffer);

	// TODO: Pick a proper return value.
	if (n == 0)
		return INPUT_BUF_EMPTY;

	game.play_buffer[n - 1] = '\0';
	return NONE;
}

void clear_buffer(Game game)
{
	memset(game.play_buffer, 0, game.word_size);
}

/*
  Wrapper for check_word. Checks the word in game.play_buffer.
*/
GameError check(Game game)
{
	return check_word(game, game.play_buffer);
}

/*
  Check whether the word is playable.
*/
GameError check_word(Game game, const char *word)
{
	if (!is_word_alpha(word))
		return NOT_ALPHA;
	if (!is_similar(word, game.letters))
		return WRONG_LETTER;
	if (word_exists(game.played_words, word))
		return WORD_PLAYED;
	if (!word_exists(game.words, word))
		return INCORRECT_WORD;

	return NONE;
}

// TODO: Clean up the interface to the word counter functions.
//       Ideally we'd want function overloading and have something like:
//       word_counts(Game)  word_counts(TrieNode, const char*)

internal void possible_word_counter(TrieNode *root, const char *letters, unsigned int *counts, int depth)
{
	bool seen[ALPHABET_SIZE] = {false};
	
	for (int i = 0; i < strlen(letters); i++) {
		int index = letters[i] - 'A';
		if (seen[index])
			continue;
		seen[index] = true;
		if (root->children[index]) {
			if (root->children[index]->is_word) {
				counts[depth]++;
			}

			char *l = remove_letter(letters, i);
			possible_word_counter(root->children[index], l, counts, depth + 1);
			free(l);
		}
	}
}

/*
  Same as possible_word_count() except the returned array is the word count of
  words played by the player, NOT the possible words that the player can play.
*/
unsigned int *played_word_count(Game game)
{
	unsigned int *counts = calloc(game.word_size, sizeof(unsigned int));
	assert(counts);
	possible_word_counter(game.played_words, game.letters, counts, 0);
	return counts;
}

/*
  This function goes through the trie structure and counts how many possible words
  can be made given the random letters from game.letters.

  Returns an integer array of the words whose char length is the array's index.
*/ 
unsigned int *possible_word_count(Game game)
{
	unsigned int *counts = calloc(game.word_size, sizeof(unsigned int));

	assert(counts);
	possible_word_counter(game.words, game.letters, counts, 0);

	return counts;
}
