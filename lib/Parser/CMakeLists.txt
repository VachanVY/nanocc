add_library(nanoccParser
    Parser.cpp
)

target_include_directories(nanoccParser PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(nanoccParser PUBLIC
    nanoccLexer
)
