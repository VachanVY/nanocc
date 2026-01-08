#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include "regex.h"
#include "lexer_internal.h"
#include "lexer.h"

CTokenSpec token_specs[] = {{INT, int_fsm},
                            {VOID, void_fsm},
                            {RETURN, return_fsm},
                            {IF, if_fsm},
                            {ELSE, else_fsm},
                            {QUESTION, question_fsm},
                            {COLON, colon_fsm},
                            {DO, do_fsm},
                            {WHILE, while_fsm},
                            {FOR, for_fsm},
                            {BREAK, break_fsm},
                            {CONTINUE, continue_fsm},

                            {TILDE, tilde_fsm},
                            {DECREMENT, decrement_fsm},
                            {NOT, not_fsm},
                            {MINUS, minus_fsm},
                            {PLUS, plus_fsm},
                            {STAR, star_fsm},
                            {SLASH, slash_fsm},
                            {PERCENT, percent_fsm},
                            {AND, and_fsm},
                            {OR, or_fsm},
                            {ASSIGN, assign_fsm},
                            {EQUAL, equal_fsm},
                            {NOT_EQUAL, not_equal_fsm},
                            {LESSTHAN, lessthan_fsm},
                            {GREATERTHAN, greaterthan_fsm},
                            {LESS_EQUAL, less_equal_fsm},
                            {GREATER_EQUAL, greater_equal_fsm},
                            {IDENTIFIER, identifier_fsm},
                            {CONSTANT, constant_fsm},
                            {LPAREN, lparen_fsm},
                            {RPAREN, rparen_fsm},
                            {LBRACE, lbrace_fsm},
                            {RBRACE, rbrace_fsm},
                            {SEMICOLON, semicolon_fsm},
                            {COMMA, comma_fsm}};
int NUMFMA = sizeof(token_specs) / sizeof(CTokenSpec);

CTokenVec clexer(char* s, size_t slen, bool debug) {
    // init all members to 0/NULL
    CTokenVec tokens = {0};
    size_t pos = 0;

    bool matched;
    size_t match_length;
    CTokenType class_type;
    while (pos < slen) {
        if (isspace(s[pos])) {
            pos++;
            continue;
        }

        match_length = 0;
        class_type = INVALID;
        matched = false;
        for (int i = 0; i < NUMFMA; i++) {
            char* temp_match;
            char* remainder = s + pos; // s[pos:]
            int curr_match_length;
            if ((curr_match_length = token_specs[i].second(remainder)) != 0) {
                // what if they are equal? how to decide? keep the first one
                if (curr_match_length > match_length) {
                    class_type = token_specs[i].first;
                    match_length = curr_match_length;
                    matched = true;
                }
            }
        }

        assert(class_type != INVALID);
        if (!matched) {
            LexerRaiseError("Lexical error at position %zu: unexpected character '%c'\n", pos,
                            s[pos]);
        }

        // add to `tokens`
        tokenPushBack(tokens, class_type, s + pos, match_length);

        // remove substr of the string now that it is tokenized
        pos += match_length;
    }
    if (debug) {
        printf("----- Lexical Analysis -----\n");
        tokensPrint(tokens);
        printf("----------------------------\n");
    }
    return tokens;
}

/*
int main() {
    char* fbuf = read_file("lextestfile.c");

    CTokenVec tokens = clexer(fbuf, strlen(fbuf), false);
    tokensPrint(tokens);

    free(fbuf);
    return 0;
}
*/