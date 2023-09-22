#include "codegen.h"
#include "parser.h"
#include "scanner.h"

using namespace lev::codegen;
using namespace lev::parser;

using namespace llvm;

using Codegen = lev::codegen::Codegen;

auto Codegen::compile(std::string_view source) -> void {
  Parser parser(source);

  auto statements = parser.parse();
  if (not statements) {
    Parser::printError(statements.error());
    return;
  }
  for (auto& statement : statements.value()) {
    statement->visit(*this);
  }
}

Codegen::Codegen() {
  mContext = std::make_unique<LLVMContext>();
  mModule = std::make_unique<Module>("lev", *mContext);
  mBuilder = std::make_unique<IRBuilder<>>(*mContext);
}

auto Codegen::convertType(ast::Type type) -> llvm::Type* {
  llvm::Type* llvmType = nullptr;
  switch (type) {
    case Type::i8:
    case Type::u8:
      llvmType  = mBuilder->getInt8Ty();
      break;
    case Type::i16:
    case Type::u16:
      llvmType = mBuilder->getInt16Ty();
      break;
    case Type::i32:
    case Type::u32:
      llvmType = mBuilder->getInt32Ty();
      break;
    case Type::i64:
    case Type::u64:
      llvmType = mBuilder->getInt64Ty();
      break;
    case Type::f32:
      llvmType = mBuilder->getFloatTy();
      break;
    case Type::f64:
      llvmType = mBuilder->getDoubleTy();
      break;
    default:
      llvmType = mBuilder->getVoidTy();
  };

  return llvmType;
}

auto Codegen::visit(ExprStmt& e) -> void {

}

auto Codegen::visit(FunctionDeclaration& f) -> void {
  std::vector<llvm::Type*> args = {};
  for (auto arg : f.args) {
    auto type = convertType(std::get<1>(arg));
    args.push_back(type);
  }

  llvm::Type* type = convertType(f.returnType);

  auto* funcType = FunctionType::get(type, args, false);
  mCurrentFunction = Function::Create(funcType, llvm::Function::ExternalLinkage, f.functionName, *mModule);

  for (auto& arg : mCurrentFunction->args()) {
    arg.setName(std::get<0>(f.args[arg.getArgNo()]));
  }

  auto* block = BasicBlock::Create(*mContext, "entry", mCurrentFunction);
  mBuilder->SetInsertPoint(block);

  for (auto& stmts : f.body) {
    stmts->visit(*this);
  }
}

auto Codegen::visit(VariableDeclaration& v) -> void {
  if (mCurrentFunction == nullptr) {
    mModule->getOrInsertGlobal(v.identifier.lexeme, convertType(v.type));
    auto* gVar = mModule->getNamedGlobal(v.identifier.lexeme);
    gVar->setLinkage(GlobalValue::CommonLinkage);
    gVar->setAlignment(Align(4));
    return;
  } else {
  }
}

auto Codegen::dump() -> std::string {
  std::string str;
  llvm::raw_string_ostream OS(str);

  Codegen codegen;

  OS << *mModule;
  OS.flush();
  return str;
}
