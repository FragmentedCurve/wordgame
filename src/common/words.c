#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>

#include <common.h>

bool is_word_alpha(const char *word)
{
	while (*word) {
		if (!isalpha(*word))
			return false;
		word++;
	}
	return true;
}

bool in_word(char c, const char *word)
{
	while (*word) {
		if (*word == c)
			return true;
		word++;
	}
	return false;
}

int in_word_count(char c, const char *word)
{
	int count = 0;
	
	while (*word) {
		if (*word == c)
			count++;
		word++;
	}
	return count;
}

bool is_similar(const char *word1, const char *word2)
{
	char l1[ALPHABET_SIZE] = {0};
	char l2[ALPHABET_SIZE] = {0};

	while (*word1) {
		char index = toupper(*word1) - 'A';
		l1[index]++;
		word1++;
	}

	while (*word2) {
		char index = toupper(*word2) - 'A';
		l2[index]++;
		word2++;
	}

	for (int i = 0; i < ALPHABET_SIZE; i++)
		if (l1[i] > l2[i])
			return false;
	
	return true;
}

/*
 Returns a NULL terminated string of length len.
 repeat_max can be is the maximum number of times a
 letter can be repeated in the string. 
 If repeat_max > len, then unlimited repeats
 are allowed.
*/
char *rand_letters(int len, int repeat_max)
{
	char *letters = calloc(len + 1, sizeof(char));
	srand((unsigned int) time(NULL));

	for (int i = 0; i < len; i++) {
		char c = (rand() % ALPHABET_SIZE) + 'A';
		// Allow a letter to repeat repeat_max times.
		while (in_word_count(c, letters) >= repeat_max)
			c = (rand() % ALPHABET_SIZE) + 'A';
		letters[i] = c;
	}
	
	return letters;
}
