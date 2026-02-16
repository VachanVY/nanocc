#include "TestCommon.hpp"
#include "nanocc/Lexer/Lexer.hpp"
#include "nanocc/AST/AST.hpp"
#include "nanocc/Parser/Parser.hpp"
#include "nanocc/Sema/Sema.hpp"
#include "nanocc/IR/IR.hpp"

int main(int argc, char* argv[]) {
    auto args = nanocc::test::parseTestArgs(argc, argv);
    auto contents = nanocc::getFileContents(args.filename);
    auto tokens = nanocc::lexer(contents, args.debug);
    auto ast = nanocc::parse(tokens, args.debug);
    nanocc::semanticAnalysis(ast, args.debug);
    auto ir = nanocc::generateIntermRepr(ast, args.debug);
    return 0;
}