add_library(nanoccIR
    IR.cpp
    IRGen.cpp
    IRDump.cpp
)

target_include_directories(nanoccIR PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(nanoccIR PUBLIC
    nanoccSema
)
