
#include <lev/compilation_unit.h>
#include <lev/lex/lex.h>
#include <lev/source.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/MemoryBuffer.h>

auto main(int argc, char** argv) -> int {
  namespace cl = llvm::cl;

  /// Compile Stuff
  enum CompilePhase {
    lex,
    parse,
    lower,
  };

  cl::SubCommand compile("compile", "compile a lev source code");
  cl::opt<std::string> input_filename(cl::Positional, cl::desc("input file"),
                                      cl::value_desc("filename"),
                                      cl::sub(compile), cl::Required);

  cl::opt<CompilePhase> phase(
      "phase",
      cl::values(clEnumVal(lex, "Lex only"), clEnumVal(parse, "Parse only"),
                 clEnumVal(lower, "Lower to LLVM")),
      cl::sub(compile));

  cl::opt<bool> dump_tokens("dump-tokens", cl::sub(compile));

  cl::ParseCommandLineOptions(argc, argv, "lev");

  if (compile) {
    auto file = llvm::MemoryBuffer::getFile(input_filename, true);

    if (!file) {
      llvm::errs() << "Invalid file provided: " << input_filename;
      return 1;
    }

    auto source = Source(input_filename, std::move(*file));
    auto unit = Lev::CompilationUnit(std::move(source));

    unit.RunLexer();

    if (dump_tokens) {
      unit.DumpTokens();
    }
  }

  return 0;
}
