#include "codegen.h"
#include "parser.h"
#include "scanner.h"

#include <string>

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
  std::vector<llvm::Type*> argsType = {};
  for (auto arg : f.args) {
    auto type = convertType(std::get<1>(arg));
    argsType.push_back(type);
  }

  llvm::Type* returnType = convertType(f.returnType);

  auto* funcType = FunctionType::get(returnType, argsType, false);
  auto* function = Function::Create(funcType, llvm::Function::ExternalLinkage, f.functionName, *mModule);

  for (auto& arg : function->args()) {
    arg.setName(std::get<0>(f.args[arg.getArgNo()]));
  }

  auto* block = BasicBlock::Create(*mContext, "entry", function);
  mBuilder->SetInsertPoint(block);

  for (auto& stmt : f.body) {
    stmt->visit(*this);
  }
}

auto Codegen::visit(VariableDeclaration& v) -> void {
  auto name = v.identifier.lexeme;
  auto type = convertType(v.type);
  auto value = visit(*v.value->as<LiteralExpr*>(), v.type);

  if (mBuilder->GetInsertBlock() == nullptr) {
    mVariables[name] = new GlobalVariable(*mModule, type, false, GlobalValue::ExternalLinkage, value, name);
    return;
  }
  auto* alloca = mBuilder->CreateAlloca(type, nullptr, v.identifier.lexeme);
  mBuilder->CreateStore(value, alloca);
  mVariables[name] = alloca;
}

auto Codegen::visit(LiteralExpr& e) -> void {
  
};

auto Codegen::visit(LiteralExpr& e, ast::Type type) -> llvm::Constant* {
  switch (type) {
    case Type::i8:
    case Type::u8:
      return mBuilder->getInt8(std::stoi(std::string(e.token.lexeme)));
    case Type::i16:
    case Type::u16:
      return mBuilder->getInt16(std::stoi(std::string(e.token.lexeme)));
    case Type::i32:
    case Type::u32:
      return mBuilder->getInt32(std::stoi(std::string(e.token.lexeme)));
    case Type::i64:
    case Type::u64:
      return mBuilder->getInt64(std::stoi(std::string(e.token.lexeme)));
    case Type::f32:
      return ConstantFP::get(*mContext, APFloat(std::stof(std::string(e.token.lexeme))));
    case Type::f64:
      return ConstantFP::get(*mContext, APFloat(std::stod(std::string(e.token.lexeme))));
    default:
      return ConstantFP::get(*mContext, APFloat(0.0));
  }
}

auto Codegen::visit(UnaryExpr& e) -> void {};
auto Codegen::visit(BinaryExpr& e) -> void {};

auto Codegen::dump() -> std::string {
  std::string str;
  llvm::raw_string_ostream OS(str);

  Codegen codegen;

  OS << *mModule;
  OS.flush();
  return str;
}
