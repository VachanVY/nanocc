#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "lexer_internal.h"
#include "regex.h"

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
                            {EXTERN, extern_fsm},
                            {STATIC, static_fsm},

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
const int NUMFMA = sizeof(token_specs) / sizeof(CTokenSpec);

/* `# <line_num> "<filename>`" ...*/
size_t parse_filename_lineno(char* s, size_t* lineno, char** filename) {
  assert(s[0] == '#');

  char* orig = s;
  s += 2; // skip `# `
  *lineno = atol(s);

  char* start = strchr(s, '"') + 1;
  char* end = strchr(start, '"');
  size_t len = (size_t)(end - start) + 1;

  static size_t capacity = 0;
  if (len > capacity) {
    *filename = (char*)realloc(*filename, len);
    capacity = len;
    if (!*filename) {
      perror("`realloc` failed in `parse_filename_lineno`");
      exit(1);
    }
  }
  memcpy(*filename, start, len - 1);
  (*filename)[len - 1] = '\0';
  return (size_t)(strchr(orig, '\n') - orig) + 1;
}

CTokenVec clexer(char* s, size_t slen, bool debug) {
  // init all members to 0/NULL
  CTokenVec tokens = {0};
  size_t pos = 0;

  bool matched;
  size_t match_length;
  CTokenType class_type;

  size_t lineno = 1;
  size_t columnno = 1;

  char* curr_filename = NULL;
  while (pos < slen) {
    if (s[pos] == '#') {
      pos += parse_filename_lineno(s + pos, &lineno, &curr_filename);
      columnno = 1;
      continue;
    }
    if (isspace(s[pos])) {
      if (s[pos] == '\n') {
        lineno++;
        columnno = 1;
      }
      pos++;
      continue;
    }

    // store the longest match length; reset for each new position
    match_length = 0;
    class_type = INVALID;
    matched = false;
    for (int i = 0; i < NUMFMA; i++) {
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
      LexerRaiseError(
          "Lexical error at position %zu: unexpected character '%c'\n", pos,
          s[pos]);
    }

    // add to `tokens`
    CTokenLocation location = {curr_filename, lineno, columnno};
    tokenPushBack(tokens, class_type, s + pos, match_length, location);

    // remove substr of the string now that it is tokenized
    pos += match_length;
    columnno++;

  }
  if (debug) {
    printf("----- Lexical Analysis -----\n");
    tokensPrint(tokens);
    printf("----------------------------\n");
  }
  return tokens;
}
