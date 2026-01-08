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

/*
/// @brief reads a file; don't forget to free the buffer
/// @param filename
/// @return file contents
char* read_file(const char* filename) {
    FILE* fptr = fopen(filename, "rb");
    if (!fptr) { // file couldn't open
        LexerRaiseError("Lexer Error: Failed to read file!\n");
    }

    // The  fseek() function sets the file position indicator for the stream pointed to by stream.
    // int fseek(FILE *stream, long offset, int origin);
    // stream: which file || offset: how far to move || origin: from where you apply that offset
    fseek(fptr, 0, SEEK_END);

    // curr position
    long size = ftell(fptr);
    // move back fptr to start
    rewind(fptr);

    char* buff = (char*)malloc(sizeof(char) * size);
    if (!buff) {
        LexerRaiseError("Lexer Error: Failed to allocate memory for reading file!\n");
    }

    // size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
    // ptr - where the data will be stored (a pointer to memory)
    // size - size of each element
    // nmemb - how many elements to read
    // stream - which file to read from
    fread(buff, 1, size, fptr);
    buff[size] = '\0';

    fclose(fptr);
    return buff;
}
*/