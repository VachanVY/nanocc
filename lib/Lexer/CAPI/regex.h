#pragma once

// keyword FSMs
int int_fsm(const char* s);
int void_fsm(const char* s);
int return_fsm(const char* s);
int if_fsm(const char* s);
int else_fsm(const char* s);
int question_fsm(const char* s);
int colon_fsm(const char* s);
int do_fsm(const char* s);
int while_fsm(const char* s);
int for_fsm(const char* s);
int break_fsm(const char* s);
int continue_fsm(const char* s);
int extern_fsm(const char* s);
int static_fsm(const char* s);

// "unary operators"
int tilde_fsm(const char* s);
int decrement_fsm(const char* s);
int not_fsm(const char* s);

// both unary and binary
int minus_fsm(const char* s);

// "binary operators"
int plus_fsm(const char* s);
int star_fsm(const char* s);
int slash_fsm(const char* s);
int percent_fsm(const char* s);
int and_fsm(const char* s);
int or_fsm(const char* s);

// assignment operators // comes under binary operators but has less precedence
int assign_fsm(const char* s);

int equal_fsm(const char* s);
int not_equal_fsm(const char* s);
int lessthan_fsm(const char* s);
int greaterthan_fsm(const char* s);
int less_equal_fsm(const char* s);
int greater_equal_fsm(const char* s);

/// @brief `std::regex("[a-zA-Z_]\\w*\\b")` | variable/function names
/// @param s
/// @return match length if it matches else 0
int identifier_fsm(const char* s);

/// @brief `std::regex("[0-9]+\\b")`
/// @param s
/// @return match length if it matches else 0
int constant_fsm(const char* s);
int lparen_fsm(const char* s);
int rparen_fsm(const char* s);
int lbrace_fsm(const char* s);
int rbrace_fsm(const char* s);
int semicolon_fsm(const char* s);

// function args separator
int comma_fsm(const char* s);
