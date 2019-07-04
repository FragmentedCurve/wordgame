#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <assert.h>
#include <common.h>

internal char *strdup(const char *str)
{
	int n = strlen(str);
	char *s = calloc(n + 1, sizeof(char));
	assert(s);
	
	for (int i = 0; i < n; i++)
		s[i] = str[i];
	
	return s;
}

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
	TrieNode *words;
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

void reset_game(Game *game, unsigned int word_size)
{
	free_trienode(game->played_words);
	free(game->letters);
	game->word_size = word_size;
	game->played_words = new_trienode();
	game->letters = rand_letters(game->word_size, game->repeat_max);
}

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

GameError input_char(Game game, char c)
{
	int n = strlen(game.letters);
	
	if (n == game.word_size)
		return INPUT_FULL;

	game.letters[n] = c;
	return NONE;
}

GameError check(Game game)
{
	return check_word(game, game.play_buffer);
}

GameError check_word(Game game, const char *word)
{
	if (!is_word_alpha(word))
		return NOT_ALPHA;
	if (!is_similar(word, game.letters))
		return WRONG_LETTER;
	if (word_exists(game.played_words, word))
		return WORD_PLAYED;

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

unsigned int *played_word_count(Game game)
{
	unsigned int *counts = calloc(game.word_size, sizeof(unsigned int));
	assert(counts);
	possible_word_counter(game.played_words, game.letters, counts, 0);
	return counts;
}

unsigned int *possible_word_count(Game game)
{
	unsigned int *counts = calloc(game.word_size, sizeof(unsigned int));

	assert(counts);
	possible_word_counter(game.words, game.letters, counts, 0);

	return counts;
}
