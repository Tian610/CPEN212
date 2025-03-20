#include "trietest.h"

int main(void) {
    printf("Tests begin\n");

    trie_node_t* p = trie_new();

    bool valid = trie_lookup(p, "");
    printf("0 Null lookup is: %d\n",valid);
    
    trie_insert(p, "hello");
    trie_insert(p, "hello friend");
    trie_insert(p, "hell");
    trie_insert(p, "helios");
    trie_delete(p, "hello");
    printf("0 hello lookup is: %d\n", trie_lookup(p, "hello"));
    printf("1 hell lookup is: %d\n", trie_lookup(p, "hell"));
    printf("1 helios lookup is: %d\n", trie_lookup(p, "helios"));
    printf("1 friend lookup is: %d\n", trie_lookup(p, "hello friend"));

}