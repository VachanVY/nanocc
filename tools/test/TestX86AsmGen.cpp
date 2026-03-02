#include "TestCommon.hpp"
#include "nanocc/AST/AST.hpp"
#include "nanocc/Codegen/IRToPseudoAsmPass.hpp"
#include "nanocc/IR/IR.hpp"
#include "nanocc/Lexer/Lexer.hpp"
#include "nanocc/Parser/Parser.hpp"
#include "nanocc/Sema/Sema.hpp"
#include "nanocc/Target/X86/X86TargetEmitter.hpp"

int main(int argc, char *argv[]) {
  auto args = nanocc::test::parseTestArgs(argc, argv);

  // Derive output paths from input filename (e.g. /path/to/foo.c ->
  // /path/to/foo)
  std::filesystem::path source_path(args.filename);
  std::filesystem::path output_binary =
      source_path.parent_path() / source_path.stem();
  std::filesystem::path output_object =
      source_path.parent_path() / (source_path.stem().string() + ".o");
  std::filesystem::path output_asm =
      source_path.parent_path() / (source_path.stem().string() + ".s");

  auto contents = nanocc::getFileContents(args.filename);

  // --- Lex ---
  auto tokens = nanocc::lexer(contents, args.debug);
  if (args.exit_stage == nanocc::test::ExitStage::Lex)
    return 0;

  // --- Parse ---
  auto ast = nanocc::parse(tokens, args.debug);
  if (args.exit_stage == nanocc::test::ExitStage::Parse)
    return 0;

  // --- Semantic analysis (validate) ---
  nanocc::semanticAnalysis(ast, args.debug);
  if (args.exit_stage == nanocc::test::ExitStage::Validate)
    return 0;

  // --- IR generation (tacky) ---
  auto ir = nanocc::generateIntermRepr(ast, args.debug);
  if (args.exit_stage == nanocc::test::ExitStage::Tacky)
    return 0;

  // --- Code generation ---
  auto pseudo_asm = nanocc::intermReprToPseudoAsm(ir, args.debug);
  nanocc::x86CorrectAssembly(pseudo_asm, args.debug);
  auto output = nanocc::x86EmitAssembly(pseudo_asm);
  auto output_str = output.str();
  if (args.exit_stage == nanocc::test::ExitStage::Codegen)
    return 0;

  if (args.debug) {
    std::println("\n\n|| Generated Assembly ||:\n\n{}", output_str);
  }

  // --- Emit assembly file (-S) ---
  if (args.exit_stage == nanocc::test::ExitStage::Assembly) {
    std::ofstream asm_out(output_asm);
    if (!asm_out) {
      std::println(stderr, "Error: Failed to write assembly to {}",
                   output_asm.string());
      return 1;
    }
    asm_out << output_str;
    return 0;
  }

  // --- Compile to object file (-c) ---
  if (args.exit_stage == nanocc::test::ExitStage::ObjectFile) {
    bool ok = nanocc::test::assembleToFile(output_str, output_object, true,
                                           args.debug);
    return ok ? 0 : 1;
  }

  // --- Full compilation: produce executable ---
  bool ok = nanocc::test::assembleToFile(output_str, output_binary, false,
                                         args.debug);
  return ok ? 0 : 1;
}