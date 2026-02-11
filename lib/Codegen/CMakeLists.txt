add_library(nanoccCodegen
    IRToPseudoAsmPass.cpp
)

target_include_directories(nanoccCodegen PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(nanoccCodegen PUBLIC
    nanoccIR
)
