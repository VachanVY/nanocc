#pragma once

#include "nanocc/Utils.hpp"
#include <string>

namespace nanocc::test {

struct TestArgs {
    std::string filename;
    bool debug = true;
};

inline TestArgs parseTestArgs(int argc, char* argv[]) {
    if (argc < 2) {
        std::println(stderr, "Usage: {} <source_file>", argv[0]);
        std::exit(1);
    }

    TestArgs args;
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (!arg.starts_with("-")) {
            args.filename = arg;
            break;
        }
    }

    if (args.filename.empty()) {
        std::println(stderr, "Error: No source file specified");
        std::exit(1);
    }

    return args;
}

} // namespace nanocc::test
