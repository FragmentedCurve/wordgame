#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>

#include <common.h>

TrieNode *new_trienode()
{
	TrieNode *node = calloc(1, sizeof(TrieNode));
	assert(node);

	node->is_word= false;
	
	return node;
}

void free_trienode(TrieNode *root)
{
	if (!root)
		return;
	
	for (int i = 0; i < ALPHABET_SIZE; i++) {
		if (root->children[i])
			free_trienode(root->children[i]);
	}
	free(root);	
}

void insert_word(TrieNode *trie, const char *word)
{
	assert(trie);
	
	while (*word) {
		int i = toupper(*word) - 'A';
		assert(i < ALPHABET_SIZE);
		if (!trie->children[i]) {
			trie->children[i] = new_trienode();
		}
		trie = trie->children[i];
		word++;
	}

	trie->is_word = true;
}

bool word_exists(TrieNode *trie, const char *word)
{
	while (*word) {
		int i = toupper(*word) - 'A';
		if (trie->children[i])
			trie = trie->children[i];
		else
			return false;
		word++;
	}

	if (trie->is_word)
		return true;
	return false;
}
