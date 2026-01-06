#include "nanocc/Utils.hpp"
#include "nanocc/Sema/Sema.hpp"

namespace nanocc {
    TypeCheckerSymbolTable global_type_checker_map;

    void semanticAnalysis(std::unique_ptr<ProgramNode>& ast, bool debug){
        nanocc::semaIdentifierResolution(ast);
        if (debug) {
            std::println("----- Identifier Resolution -----");
            ast->dump();
            std::println("---------------------------------");
        }
        nanocc::semaCheckTypes(ast, global_type_checker_map);
        if (debug) {
            std::println("----- Type Checking -----");
            ast->dump();
            std::println("-------------------------");
        }

        nanocc::semaLoopLabelling(ast);
        if (debug) {
            std::println("----- Loop Labelling -----");
            ast->dump();
            std::println("--------------------------");
        }
    }
} // namespace nanocc
