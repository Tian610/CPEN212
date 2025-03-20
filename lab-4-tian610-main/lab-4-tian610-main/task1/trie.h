#ifndef __TRIE_H__
#define __TRIE_H__

#include <stdbool.h>

// a node in a trie that implements a set of strings
//
// the entry terminating at this node exists iff valid is true
// the next level for that child exists iff the child is not NULL
// nodes are not shared within the same trie or among different tries
//
// trie invariant: if valid = false, there is at least one descendant node
//                 (child, grandchild, etc.) with valid = true
// (it follows from this, for example, that leaves always have valid = true)
typedef struct trie_node {
    struct trie_node *children[256];
    bool valid;
} trie_node_t;

// description:
// - allocates a new trie that contains only the empty string
// returns:
// - pointer p to the top-level trie_node_t in the new trie
// - p != NULL unless malloc fails to allocate sizeof(trie_node_t) bytes
// output invariants:
// - the trie invariant is maintained
// - exactly sizeof(trie_node_t) bytes have been allocated via malloc()
trie_node_t *trie_new();

// description:
// - finds a string in a trie
// arguments:
// - t: pointer to the top-level trie_node_t in the trie
// - s: a zero-terminated C string to be looked up in the trie
// returns:
// - true if and only if s is in the trie
// input invariants:
// - t != NULL
// - s != NULL
// output invariants:
// - the trie is not modified
// - no heap memory is allocated or freed
bool trie_lookup(const trie_node_t *t, const char *s);

// description:
// - inserts a string into a trie
// arguments:
// - t: pointer to the top-level trie_node_t in the trie
// - s: a zero-terminated C string to be inserted in the trie
// input invariants:
// - t != NULL
// - s != NULL
// - *s != '\0'
// output invariants:
// - looking up s in the resulting trie returns true
// - the trie invariant is maintained
// - if the string was already in the trie
//   - the trie is not modified
//   - no heap memory is allocated
// - at most sizeof(trie_node_t) * (strlen(s) - N) bytes of memory are allocated,
//   where N is the longest common prefix of s and any string already in the trie
// - no heap memory is freed
void trie_insert(trie_node_t *t, const char *s);

// description:
// - deletes a string from a trie
// arguments:
// - t: pointer to the top-level trie_node_t in the trie
// - s: a zero-terminated C string to be deleted from the trie
// input invariants:
// - t != NULL
// - s != NULL
// - *s != '\0'
// output invariants:
// - looking up s in the resulting trie returns false
// - the trie invariant is maintained
// - any nodes removed from the trie are freed
// - no heap memory is allocated
// - if the string was not in the trie
//   - the trie is not modified
//   - no heap memory is freed
void trie_delete(trie_node_t *t, const char *s);

#endif // __TRIE_H__
