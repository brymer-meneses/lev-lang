#include "codegen.h"
#include "parser.h"
#include "scanner.h"

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

  mStatements = std::move(statements.value());
}

Codegen::Codegen(std::vector<std::unique_ptr<Stmt>> statements)  {
  mStatements = std::move(statements);
  mContext = std::make_unique<LLVMContext>();
  mModule = std::make_unique<Module>("lev", *mContext);
  mBuilder = std::make_unique<IRBuilder<>>(*mContext);
};

auto Codegen::compile() -> void {
  for (auto& statement : mStatements){
    codegenStmt(*statement);
  }
}

auto Codegen::codegenExpr(Expr& expr) -> llvm::Value* {
  setCurrentExpr(&expr);
  expr.accept(*this);
  return mEvaluatedExpr;
}

auto Codegen::codegenStmt(Stmt& stmt) -> void {
  mCurrentStmt = &stmt;
  stmt.accept(*this);
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
    auto name = std::get<0>(f.args[arg.getArgNo()]);
    arg.setName(name);
  }

  auto* block = BasicBlock::Create(*mContext, "entry", function);
  mBuilder->SetInsertPoint(block);

  for (auto& stmt : f.body) {
    codegenStmt(*stmt);
  }
}

auto Codegen::visit(VariableDeclaration& v) -> void {
  auto type = convertType(v.type);

  if (mBuilder->GetInsertBlock() == nullptr) {
    mModule->getOrInsertGlobal(v.identifier.lexeme, convertType(v.type));
    auto* globalVariable = mModule->getNamedGlobal(v.identifier.lexeme);
    globalVariable->setLinkage(GlobalValue::CommonLinkage);
    globalVariable->setAlignment(Align(4));
    globalVariable->setConstant(v.isMutable);

    auto* value = codegenExpr(*v.value);
    globalVariable->setInitializer((Constant*) value);
    return;
  }

  auto* alloca = mBuilder->CreateAlloca(type, nullptr, v.identifier.lexeme);
  auto* value = codegenExpr(*v.value);
  mBuilder->CreateStore(value, alloca);
}

auto Codegen::visit(LiteralExpr& e) -> void {

  switch (e.token.type) {
    case TokenType::Integer:
      setEvaluatedExpr(ConstantInt::get(mBuilder->getInt32Ty(), std::stoi(std::string(e.token.lexeme))));
      break;
    case TokenType::Float:
      setEvaluatedExpr(ConstantFP::get(mBuilder->getFloatTy(), std::stod(std::string(e.token.lexeme))));
      break;
    case TokenType::Identifier:
      // variable lookup
    default:
      std::cerr << "codegen not implemented\n";
      exit(0);
      break;
  }
};

auto Codegen::visit(UnaryExpr& e) -> void {};
auto Codegen::visit(BinaryExpr& e) -> void {};

auto Codegen::dump() -> std::string {
  std::string str;
  llvm::raw_string_ostream OS(str);

  OS << *mModule;
  OS.flush();
  return str;
}
