add_library(nanoccLexer
    Lexer.cpp
    CAPI/lexer.c
    CAPI/regex.c
)

target_include_directories(nanoccLexer PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)
