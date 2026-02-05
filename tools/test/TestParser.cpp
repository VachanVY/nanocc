#include "TestCommon.hpp"
#include "nanocc/Lexer/Lexer.hpp"
#include "nanocc/AST/AST.hpp"
#include "nanocc/Parser/Parser.hpp"

int main(int argc, char* argv[]) {
    auto args = nanocc::test::parseTestArgs(argc, argv);
    auto contents = nanocc::getFileContents(args.filename);
    auto tokens = nanocc::lexer(contents, args.debug);
    auto ast = nanocc::parse(tokens, args.debug);

    return 0;
}