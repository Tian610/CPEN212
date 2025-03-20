#include <inttypes.h>
#include <stddef.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "crazylist.h"

crazycons_t *enclosing_struct(uint64_t *car) {
    return (crazycons_t *) ((void *) car - (sizeof(crazycons_t)-sizeof(car)));
}

uint64_t *cons(uint64_t car, uint64_t *cdr) {
    crazycons_t *cons = (crazycons_t *) malloc(sizeof(crazycons_t));
    assert(cons);
    cons->car = car;
    cons->cdr = cdr;
    assert(cons);
    return (uint64_t *) &cons->car;
}

uint64_t first(uint64_t *list) {
    assert(list);
    crazycons_t *cons = enclosing_struct(list);
    return cons->car;
}

uint64_t *rest(uint64_t *list) {
    assert(list);
    crazycons_t *cons = enclosing_struct(list);
    return cons->cdr;
}

uint64_t *find(uint64_t *list, uint64_t query) {
    assert(list);

    crazycons_t *cons = enclosing_struct(list);
    while (cons) {
        if (cons->car == query) {
            return &cons->car;
        }
        cons = enclosing_struct(cons->cdr);
    }

    return NULL;
}

uint64_t *insert_sorted(uint64_t *list, uint64_t n) {
    
    if (!list || n < *list) {
        return cons(n, list);
    }

    crazycons_t *con = enclosing_struct(list);
    while (con) {
        if (con->car <= n && n <= enclosing_struct(con->cdr)->car) {
            uint64_t *new = cons(n, con->cdr);
            con->cdr = new;
            return list;
        }
        con = enclosing_struct(con->cdr);
    }

    con->cdr = cons(n, NULL);
    return list;
}

uint64_t *reverse(uint64_t *list) {
    if (!list) {
        return list;
    }

    crazycons_t *con = enclosing_struct(list);
    uint64_t *prev = NULL;
    uint64_t *next = NULL;

    while (con) {
        next = con->cdr;
        con->cdr = prev;
        prev = &con->car;
        if (!next) {
            break;
        }
        con = enclosing_struct(next);
    }
    
    return &con->car;
}

void print_list(uint64_t *list) {
    assert(list);
    crazycons_t *cons = enclosing_struct(list);
    printf("[");
    while (cons) {
        printf("%lu", cons->car);
        if (cons->cdr) {
            printf(", ");
            cons = enclosing_struct(cons->cdr);
        } else {
            cons = NULL;
        }
    }
    printf("]\n");
}
