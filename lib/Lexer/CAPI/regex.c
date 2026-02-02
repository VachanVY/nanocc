#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

#include "regex.h"

/// @brief everything except `0-9`, `a-z`, `A-Z`, `_` => `\\b` is true
/// @param c
/// @return if `c` is `std::regex("\\b")` return `true` else `false`;
bool _iswordbreaker(char c) { return !(isalnum(c) || c == '_'); }

int int_fsm(const char* s) {
    if (s[0] != 'i' || s[1] != 'n' || s[2] != 't' || !_iswordbreaker(s[3])) {
        return 0;
    }
    return 3;
}

int void_fsm(const char* s) {
    if (s[0] != 'v' || s[1] != 'o' || s[2] != 'i' || s[3] != 'd' || !_iswordbreaker(s[4])) {
        return 0;
    }
    return 4;
}

int return_fsm(const char* s) {
    if (s[0] != 'r' || s[1] != 'e' || s[2] != 't' || s[3] != 'u' || s[4] != 'r' || s[5] != 'n' ||
        !_iswordbreaker(s[6])) {
        return 0;
    }
    return 6;
}

int if_fsm(const char* s) {
    if (s[0] != 'i' || s[1] != 'f' || !_iswordbreaker(s[2])) {
        return 0;
    }
    return 2;
}

int else_fsm(const char* s) {
    if (s[0] != 'e' || s[1] != 'l' || s[2] != 's' || s[3] != 'e' || !_iswordbreaker(s[4])) {
        return 0;
    }
    return 4;
}

int question_fsm(const char* s) {
    if (s[0] != '?') {
        return 0;
    }
    return 1;
}

int colon_fsm(const char* s) {
    if (s[0] != ':') {
        return 0;
    }
    return 1;
}

int do_fsm(const char* s) {
    if (s[0] != 'd' || s[1] != 'o' || !_iswordbreaker(s[2])) {
        return 0;
    }
    return 2;
}

int while_fsm(const char* s) {
    if (s[0] != 'w' || s[1] != 'h' || s[2] != 'i' || s[3] != 'l' || s[4] != 'e' ||
        !_iswordbreaker(s[5])) {
        return 0;
    }
    return 5;
}

int for_fsm(const char* s) {
    if (s[0] != 'f' || s[1] != 'o' || s[2] != 'r' || !_iswordbreaker(s[3])) {
        return 0;
    }
    return 3;
}

int break_fsm(const char* s) {
    if (s[0] != 'b' || s[1] != 'r' || s[2] != 'e' || s[3] != 'a' || s[4] != 'k' ||
        !_iswordbreaker(s[5])) {
        return 0;
    }
    return 5;
}

int continue_fsm(const char* s) {
    if (s[0] != 'c' || s[1] != 'o' || s[2] != 'n' || s[3] != 't' || s[4] != 'i' || s[5] != 'n' ||
        s[6] != 'u' || s[7] != 'e' || !_iswordbreaker(s[8])) {
        return 0;
    }
    return 8;
}

int extern_fsm(const char* s) {
    if (s[0] != 'e' || s[1] != 'x' || s[2] != 't' || s[3] != 'e' || s[4] != 'r' || s[5] != 'n' ||
        !_iswordbreaker(s[6])) {
        return 0;
    }
    return 6;
}

int static_fsm(const char* s) {
    if (s[0] != 's' || s[1] != 't' || s[2] != 'a' || s[3] != 't' || s[4] != 'i' || s[5] != 'c' ||
        !_iswordbreaker(s[6])) {
        return 0;
    }
    return 6;
}

int tilde_fsm(const char* s) {
    if (s[0] != '~') {
        return 0;
    }
    return 1;
}

int decrement_fsm(const char* s) {
    if (s[0] != '-' || s[1] != '-') {
        return 0;
    }
    return 2;
}

int not_fsm(const char* s) {
    if (s[0] != '!') {
        return 0;
    }
    return 1;
}

int minus_fsm(const char* s) {
    if (s[0] != '-') {
        return 0;
    }
    return 1;
}

int plus_fsm(const char* s) {
    if (s[0] != '+') {
        return 0;
    }
    return 1;
}

int star_fsm(const char* s) {
    if (s[0] != '*') {
        return 0;
    }
    return 1;
}

int slash_fsm(const char* s) {
    if (s[0] != '/') {
        return 0;
    }
    return 1;
}

int percent_fsm(const char* s) {
    if (s[0] != '%') {
        return 0;
    }
    return 1;
}

int and_fsm(const char* s) {
    if (s[0] != '&' || s[1] != '&') {
        return 0;
    }
    return 2;
}

int or_fsm(const char* s) {
    if (s[0] != '|' || s[1] != '|') {
        return 0;
    }
    return 2;
}

int assign_fsm(const char* s) {
    if (s[0] != '=') {
        return 0;
    }
    return 1;
}

int equal_fsm(const char* s) {
    if (s[0] != '=' || s[1] != '=') {
        return 0;
    }
    return 2;
}

int not_equal_fsm(const char* s) {
    if (s[0] != '!' || s[1] != '=') {
        return 0;
    }
    return 2;
}

int lessthan_fsm(const char* s) {
    if (s[0] != '<') {
        return 0;
    }
    return 1;
}

int greaterthan_fsm(const char* s) {
    if (s[0] != '>') {
        return 0;
    }
    return 1;
}

int less_equal_fsm(const char* s) {
    if (s[0] != '<' || s[1] != '=') {
        return 0;
    }
    return 2;
}

int greater_equal_fsm(const char* s) {
    if (s[0] != '>' || s[1] != '=') {
        return 0;
    }
    return 2;
}

/// @brief `std::regex("[a-zA-Z_]\\w*\\b")`
/// @param s
/// @return match length if it matches else 0
int identifier_fsm(const char* s) {
    int len = 0;
    if (!isalpha(s[0]) && s[0] != '_') {
        return 0;
    }
    len++;

    int i = 1;
    while (!_iswordbreaker(s[i])) {
        if (!isalnum(s[i]) && s[i] != '_') {
            return 0;
        }
        i++, len++;
    }
    return len;
}

/// @brief `std::regex("[0-9]+\\b")`
/// @param s
/// @return match length if it matches else 0
int constant_fsm(const char* s) {
    int len = 0;
    if (!isdigit(s[0])) {
        return 0;
    }
    len++;

    int i = 1;
    while (!_iswordbreaker(s[i])) {
        if (!isdigit(s[i])) {
            return 0;
        }
        i++, len++;
    }
    return len;
}

int lparen_fsm(const char* s) {
    if (s[0] != '(') {
        return 0;
    }
    return 1;
}

int rparen_fsm(const char* s) {
    if (s[0] != ')') {
        return 0;
    }
    return 1;
}

int lbrace_fsm(const char* s) {
    if (s[0] != '{') {
        return 0;
    }
    return 1;
}

int rbrace_fsm(const char* s) {
    if (s[0] != '}') {
        return 0;
    }
    return 1;
}

int semicolon_fsm(const char* s) {
    if (s[0] != ';') {
        return 0;
    }
    return 1;
}

int comma_fsm(const char* s) {
    if (s[0] != ',') {
        return 0;
    }
    return 1;
}
