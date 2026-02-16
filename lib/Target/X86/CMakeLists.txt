add_library(nanoccX86Target
    X86TargetEmitter.cpp
    X86PseudoLowering.cpp
    X86InstrFixup.cpp
)

target_include_directories(nanoccX86Target PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(nanoccX86Target PUBLIC
    nanoccCodegen
)
