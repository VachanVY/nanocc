add_library(nanocc_lib # nanocc_lib is a library
    Lexer/Lexer.cpp

    Parser/Parser.cpp

    Sema/SemaDecl.cpp
    Sema/SemaType.cpp
    Sema/SemaLabel.cpp
    Sema/Sema.cpp

    IR/IRGen.cpp
    IR/IRDump.cpp
    IR/IR.cpp

    Codegen/IRToPseudoAsmPass.cpp

    Target/X86/X86PseudoLowering.cpp
    Target/X86/X86InstrFixup.cpp
    Target/X86/X86TargetEmitter.cpp
)

# PUBLIC include/ => tools + future tests can include <nanocc/...>
target_include_directories(nanocc_lib
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)

# target_compile_options(nanocc_lib PUBLIC -Wall -Wextra)
