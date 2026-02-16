add_library(nanoccSema
    Sema.cpp
    SemaDecl.cpp
    SemaLabel.cpp
    SemaType.cpp
)

target_include_directories(nanoccSema PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(nanoccSema PUBLIC
    nanoccParser
)
