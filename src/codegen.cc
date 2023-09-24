#include "codegen.h"
#include "parser.h"
#include "scanner.h"

#include <variant>
#include <string>

using namespace lev::codegen;
using namespace lev::parser;

using namespace llvm;

using Codegen = lev::codegen::Codegen;

Codegen::Codegen(std::string_view source) {
  mContext = std::make_unique<LLVMContext>();
  mModule = std::make_unique<Module>("lev", *mContext);
  mBuilder = std::make_unique<IRBuilder<>>(*mContext);

  Parser parser(source);
  auto statements = parser.parse();
  if (not statements) {
    Parser::printError(statements.error());
    return;
  }
  mStatements = std::move(*statements);
}

Codegen::Codegen(std::vector<Stmt> statements)  {
  mStatements = std::move(statements);
  mContext = std::make_unique<LLVMContext>();
  mModule = std::make_unique<Module>("lev", *mContext);
  mBuilder = std::make_unique<IRBuilder<>>(*mContext);
};

auto Codegen::compile() -> void {
  for (auto& statement : mStatements){
    codegen(statement);
  }
}

auto Codegen::codegen(const Stmt& stmt) -> void {
  stmt.accept([this](const auto &e) -> void { visit(e); });
}

auto Codegen::codegen(const Expr& expr) -> llvm::Value* {
  return expr.accept([this](const auto& e) -> llvm::Value* { return visit(e); });
}

auto Codegen::convertType(ast::Type type) const -> llvm::Type* {
  switch (type) {
    case Type::i8:
    case Type::u8:
      return mBuilder->getInt8Ty();
      break;
    case Type::i16:
    case Type::u16:
      return mBuilder->getInt16Ty();
      break;
    case Type::i32:
    case Type::u32:
      return mBuilder->getInt32Ty();
      break;
    case Type::i64:
    case Type::u64:
      return mBuilder->getInt64Ty();
      break;
    case Type::f32:
      return mBuilder->getFloatTy();
      break;
    case Type::f64:
      return mBuilder->getDoubleTy();
      break;
    default:
      return mBuilder->getVoidTy();
  };
}

auto Codegen::visit(const Stmt::ExprStmt& e) -> void {

}

auto Codegen::visit(const Stmt::FunctionDeclarationStmt& f) -> void {
  std::vector<llvm::Type*> argsType = {};
  for (auto arg : f.args) {
    auto type = convertType(arg.second);
    argsType.push_back(type);
  }

  llvm::Type* returnType = convertType(f.returnType);

  auto* funcType = FunctionType::get(returnType, argsType, false);
  auto* function = Function::Create(funcType, llvm::Function::ExternalLinkage, f.functionName, *mModule);

  for (auto& arg : function->args()) {
    auto name = f.args[arg.getArgNo()].first;
    arg.setName(name);
  }

  auto* block = BasicBlock::Create(*mContext, "entry", function);
  mBuilder->SetInsertPoint(block);

  codegen(*f.body);
}

auto Codegen::visit(const Stmt::VariableDeclarationStmt& v) -> void {
  const auto type = convertType(v.type);

  if (mBuilder->GetInsertBlock() == nullptr) {
    mModule->getOrInsertGlobal(v.identifier.lexeme, convertType(v.type));
    auto* globalVariable = mModule->getNamedGlobal(v.identifier.lexeme);
    globalVariable->setLinkage(GlobalValue::CommonLinkage);
    globalVariable->setAlignment(Align(4));
    globalVariable->setConstant(not v.isMutable);

    auto* value = codegen(*v.value);
    globalVariable->setInitializer((Constant*) value);
    return;
  }

  auto* alloca = mBuilder->CreateAlloca(type, nullptr, v.identifier.lexeme);
  auto* value = codegen(*v.value);
  mBuilder->CreateStore(value, alloca);
}

auto Codegen::visit(const Expr::LiteralExpr& e) -> llvm::Value* {
  switch (e.value.type) {
    case TokenType::Integer:
      return ConstantInt::get(convertType(Type::i32), std::stoi(std::string(e.value.lexeme)));
      break;
    case TokenType::Float:
      return ConstantFP::get(convertType(Type::f32), std::stod(std::string(e.value.lexeme)));
      break;
    case TokenType::Identifier:
      // variable lookup
    default:
      std::cerr << "codegen not implemented\n";
      exit(0);
      break;
  }
};

auto Codegen::visit(const Expr::VariableExpr& e) -> llvm::Value* {

}

auto Codegen::visit(const Expr::BinaryExpr& e) -> llvm::Value* {

}

auto Codegen::visit(const Expr::UnaryExpr& e) -> llvm::Value* {

}

auto Codegen::visit(const Stmt::BlockStmt& e) -> void {
  for (const auto& statement : e.statements) {
    codegen(statement);
  }

}

auto Codegen::visit(const Stmt::AssignStmt& e) -> void {

}

auto Codegen::dump() const -> std::string {
  std::string str;
  llvm::raw_string_ostream OS(str);

  OS << *mModule;
  OS.flush();
  return str;
}
