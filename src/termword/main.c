#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <assert.h>

#include <common.h>
#include <dict.h>

size_t getline(char **lineptr, size_t *n, FILE *stream){
	int c;
	size_t read_count = 0;
	bool flag = true;
	
	if (!*lineptr) {
		*n = 1024;
		*lineptr = calloc(*n, sizeof(char));
		assert(*lineptr);
	}

	while (flag) {
		c = fgetc(stream);
		if (c == EOF || c == '\n')
			flag = false;
		else {
			if (read_count >= *n - 2) {
				*n *= 2;
				*lineptr = realloc(*lineptr, *n);
				assert(*lineptr);
			}
			(*lineptr)[read_count] = c;
			read_count++;
		}
	}
	(*lineptr)[read_count] = '\0';
	return read_count;
}

internal void prompt(Game game)
{
	for (int i = 0; i < game.word_size; i++)
		printf("%c ", game.letters[i]);
	printf("> ");
}

internal void run_game(Game game)
{
	unsigned int *played;
	unsigned int *possible = possible_word_count(game);
	char *word = NULL;
	size_t word_size = 0;
	bool running = true;
	
	do {
		prompt(game);
		int n = getline(&word, &word_size, stdin);

		if (n == 0) {
			played = played_word_count(game);
			for (int i = 0; i < game.word_size; i++)
				printf("%d letters: %2d/%2d\n", i + 1, played[i], possible[i]);
			free(played);
			continue;
		}

		if (!strcmp("rrr", word)) {
			reset_game(&game, game.word_size);
			free(possible);
			possible = possible_word_count(game);
			continue;
		} else if (!strcmp("sss", word)) {
			shuffle(game);
			continue;
		}
		
		switch (play_word(game, word)) {
		case NOT_ALPHA:
			puts("Only letters! No symbols or numbers!");
			break;
		case WRONG_LETTER:
			puts("I don't know what game you're playing but it's not this one. Use the given letters.");
			break;
		case WORD_PLAYED:
			puts("Do ya got alzheimer's? You played that word already.");
			break;
		case INCORRECT_WORD:
			puts("Good guess but no reward.");
			break;
		default:
			puts("Winner winner chicken dinner!");
			break;
		}
	} while(running);
}

int main(int argc, char **argv)
{
	int word_size = 6;

	Game game = {0};

	for (int i = 1; i < argc; i++) {
		if (strcmp("-w", argv[i]) == 0) {
			assert(i + 1 < argc);
			game.word_size = atoi(argv[++i]);
		}
	}

	game = new_game(wl_common6, wl_common6_len, word_size, word_size);
	run_game(game);
	
	return 0;
}
