#include "trie.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


trie_node_t *trie_new() {
    trie_node_t* node = (trie_node_t*) malloc(sizeof(trie_node_t));
    if (node) {
        node->valid = true;
        memset(node->children, 0, sizeof(node->children));
    }

    return node;
}

bool trie_lookup(const trie_node_t *t, const char *s) {
    if (!t || !s) {
        return false;
    }

    size_t s_size = strlen(s);

    const trie_node_t* current_node = t;

    for (int i = 0; i < s_size; i++) {

        unsigned char current_char = (unsigned char) s[i];

        if (!(current_node->children[current_char])) {
            return false;
        }

        current_node = current_node->children[current_char];

        if (i == s_size - 1) {
            if (current_node->valid == true) {
                return true;
            }
        }

    }

    return false;
}

void trie_insert(trie_node_t *t, const char *s) {
    if (!t || !s) {
        return;
    }

    size_t s_size = strlen(s);
    trie_node_t* current_node = t;

    for (int i = 0; i < s_size; i++) {

        unsigned char current_char = (unsigned char) s[i];

        if (current_node->children[current_char]) {
            current_node = current_node->children[current_char];
        } else {
            current_node->children[current_char] = (trie_node_t*) malloc(sizeof(trie_node_t));
            current_node->children[current_char]->valid = false;
            memset(current_node->children[current_char]->children, 0, sizeof(current_node->children[current_char]->children));
            current_node = current_node->children[current_char];
        }

        if (i == s_size - 1) {
            current_node->valid = true;
        }

    }

}

void trie_delete(trie_node_t *t, const char *s) {
    if (!t || !s) {
        return;
    }

    size_t s_size = strlen(s);
    trie_node_t* current_node = t;

    // make a fake "stack" cause i can't be bothered to actually write one
    trie_node_t* node_list[s_size];
    unsigned char char_list[s_size];

    for (int i = 0; i < s_size; i++) {
        unsigned char current_char = (unsigned char) s[i];

        // If string is not in trie, return (can't delete)
        if (!current_node->children[current_char]) {
            return;
        }

        // When we reach end of string, set to false
        if (i == s_size - 1) {
            current_node->children[current_char]->valid = false;
        }

        // update our list
        node_list[i] = current_node;
        char_list[i] = current_char;
        current_node = current_node->children[current_char];
    }

    // "pop" every element in stack, if it has no children
    // then its useless and its deleted. If we encounter something
    // with children, then just return since all elements before
    // it must exist.
    for (int i = s_size - 1; i > 0; i--) {
        for (int j = 0; j < 256; j++) {
            if (node_list[i]->children[j] != NULL || node_list[i]->valid == true) {
                return;
            }
        }
        free(node_list[i]);
        node_list[i-1]->children[char_list[i]] = NULL;
    }
}
