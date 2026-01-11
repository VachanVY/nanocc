#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "lexer.h"

void LexerRaiseError(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    exit(1);
}

static inline char* tokenTypeToString(CTokenType type) {
    switch (type) {
#define X(name, str)                                                                               \
    case name:                                                                                     \
        return str;
#include "tokens.def"
#undef X
    }
    return "";
}

// "The dynamic array guy video": https://www.youtube.com/watch?v=95M6V3mZgrI
#define tokenPushBack(tokens, type, start, match_length)                                           \
    do {                                                                                           \
        if (tokens.count >= tokens.capacity) {                                                     \
            if (tokens.capacity == 0) {                                                            \
                tokens.capacity = 256;                                                             \
            } else {                                                                               \
                tokens.capacity *= 2;                                                              \
            }                                                                                      \
            tokens.items = (CToken*)realloc(tokens.items, tokens.capacity * sizeof(CToken));       \
        }                                                                                          \
        tokens.items[tokens.count++] = (CToken){type, start, match_length};                        \
    } while (0)

#define tokensPrint(tokens)                                                                        \
    do {                                                                                           \
        printf("Tokens:\n");                                                                       \
        printf("count: %ld | capacity: %ld\n\n", tokens.count, tokens.capacity);                   \
                                                                                                   \
        for (int i = 0; i < tokens.count; i++) {                                                   \
            CToken tok = tokens.items[i];                                                          \
            printf("    %-4d %-15s %-15.*s\n", i + 1, tokenTypeToString(tok.type), tok.length,     \
                   tok.start);                                                                     \
        }                                                                                          \
    } while (0)
