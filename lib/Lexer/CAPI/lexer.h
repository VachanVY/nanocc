#pragma once

#include <stdbool.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
#define X(name, str) name,
#include "tokens.def"
} CTokenType;

typedef struct {
    CTokenType type;   // token category produced by the lexer
    const char* start; // source text matched for that token
    int length;        // length matched
} CToken;

// struct with items to hold, size of it, capacity of it
typedef struct {
    CToken* items;   // array of Tokens
    size_t count;    // number of Tokens in the Vec
    size_t capacity; // actual capacity of Vec
} CTokenVec;

typedef struct {
    CTokenType first;
    int (*second)(const char*);
} CTokenSpec;

CTokenVec clexer(char* s, size_t slen, bool debug);

#define freeCTokens(tokens)                                                                        \
    do {                                                                                           \
        free(tokens.items);                                                                        \
        tokens.capacity = 0;                                                                       \
        tokens.count = 0;                                                                          \
    } while (0)

#ifdef __cplusplus
}
#endif
