#pragma once

#include "nanocc/Utils/Utils.hpp"
#include <filesystem>
#include <fstream>
#include <string>

namespace nanocc::test {

enum class ExitStage {
  None, // compile all the way through (default)
  Lex,
  Parse,
  Validate,
  Tacky,
  Codegen,
  Assembly,  // -S: emit assembly file, no linking
  ObjectFile // -c: compile to object file, no linking
};

struct TestArgs {
  std::string filename;
  bool debug = false;
  ExitStage exit_stage = ExitStage::None;
};

inline TestArgs parseTestArgs(int argc, char* argv[]) {
  if (argc < 2) {
    std::println(stderr, "Usage: {} [options] <source_file>", argv[0]);
    std::exit(1);
  }

  TestArgs args;
  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    if (arg == "--lex") {
      args.exit_stage = ExitStage::Lex;
    } else if (arg == "--parse") {
      args.exit_stage = ExitStage::Parse;
    } else if (arg == "--validate") {
      args.exit_stage = ExitStage::Validate;
    } else if (arg == "--tacky") {
      args.exit_stage = ExitStage::Tacky;
    } else if (arg == "--codegen") {
      args.exit_stage = ExitStage::Codegen;
    } else if (arg == "-S") {
      args.exit_stage = ExitStage::Assembly;
    } else if (arg == "-c") {
      args.exit_stage = ExitStage::ObjectFile;
    } else if (!arg.starts_with("-")) {
      args.filename = arg;
    }
    // ignore unrecognized flags (e.g. optimization flags from test runner)
  }

  if (args.filename.empty()) {
    std::println(stderr, "Error: No source file specified");
    std::exit(1);
  }

  return args;
}

// Write assembly string to a temp .s file and compile it to the given output
// path. If object_only is true, produce a .o file (no linking). Returns true on
// success.
inline bool assembleToFile(const std::string& assembly,
                           const std::filesystem::path& output_path,
                           bool object_only, bool debug) {
  std::filesystem::path temp_dir = std::filesystem::temp_directory_path();
  std::filesystem::path asm_file = temp_dir / "nanocc_test.s";

  {
    std::ofstream out(asm_file);
    if (!out) {
      std::println(stderr, "Failed to create temp assembly file");
      return false;
    }
    out << assembly;
  }

  std::string compile_cmd;
  if (object_only) {
    compile_cmd = "gcc -c " + asm_file.string() + " -o " + output_path.string();
  } else {
    compile_cmd = "gcc " + asm_file.string() + " -o " + output_path.string();
  }

  if (debug) {
    std::println("Compiling: {}", compile_cmd);
  }

  if (std::system(compile_cmd.c_str()) != 0) {
    std::println(stderr, "Assembly failed\n");
    return false;
  }

  return true;
}

// Legacy helper kept for compatibility – compile and run, output to /tmp
inline bool assembleAndRunWithGcc(const std::string& assembly, bool debug) {
  std::filesystem::path temp_dir = std::filesystem::temp_directory_path();
  std::filesystem::path exe_file = temp_dir / "nanocc_test_bin";

  if (!assembleToFile(assembly, exe_file, false, debug)) {
    return false;
  }

  int status = std::system(exe_file.string().c_str());
  int code = WEXITSTATUS(status);
  if (debug) {
    std::println("Program exited with code: {}", code);
  }
  return true;
}

} // namespace nanocc::test
