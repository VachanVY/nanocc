if (BUILD_NANOCC_TEST)
    add_executable(nanocc_lex test/TestLexer.cpp)
    target_link_libraries(nanocc_lex PRIVATE nanoccLexer nanoccUtils)

    add_executable(nanocc_parse test/TestParser.cpp)
    target_link_libraries(nanocc_parse PRIVATE nanoccParser nanoccUtils)

    add_executable(nanocc_sema test/TestSema.cpp)
    target_link_libraries(nanocc_sema PRIVATE nanoccSema nanoccUtils)

    add_executable(nanocc_ir test/TestIR.cpp)
    target_link_libraries(nanocc_ir PRIVATE nanoccIR nanoccUtils)

    add_executable(nanocc_codegen test/TestX86AsmGen.cpp)
    target_link_libraries(nanocc_codegen PRIVATE nanoccCodegen nanoccX86Target nanoccTransforms nanoccUtils)
else()
    add_executable(nanocc NanoCC.cpp)
    target_link_libraries(nanocc PRIVATE nanoccX86Target nanoccCodegen nanoccTransforms nanoccIR nanoccSema nanoccParser nanoccLexer nanoccUtils)
endif()

# PUBLIC would work but is wrong
# Public means: Any target that depends on nanocc should also depend on nanocc_lib
# But nanocc is the final executable, nothing depends on it
