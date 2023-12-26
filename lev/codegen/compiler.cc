#include "misc/match.h"
#include <lev/codegen/compiler.h>
#include <lev/misc/macros.h>

using namespace lev;

Compiler::Compiler(std::vector<Stmt> statements) : mStatements(std::move(statements)) {
  mContext = std::make_shared<llvm::LLVMContext>();
  mModule = std::make_unique<llvm::Module>("lev", *mContext);
  mBuilder = std::make_shared<llvm::IRBuilder<>>(*mContext);

  mSemanticContext = Context(mBuilder);
}

auto Compiler::dump() const -> std::string {
  std::string str;
  llvm::raw_string_ostream OS(str);

  OS << *mModule;
  OS.flush();
  return str;
}

auto Compiler::getModule() -> std::unique_ptr<llvm::Module> {
  return std::move(mModule);
}

auto Compiler::compile() -> std::expected<void, CodegenError> {
  auto& globalScope = mSemanticContext.createScope();
  for (const auto& statement : mStatements) {
    globalScope.addContext(&statement);
    auto status = codegen(statement);
    if (not status) {
      return std::unexpected(status.error());
    }
  }
  return {};
}

auto Compiler::codegen(const Stmt& s) -> std::expected<void, CodegenError> {
  return s.accept([this](const auto& s){ return codegen(s); });
}

auto Compiler::codegen(const Expr& e) -> std::expected<llvm::Value*, CodegenError> {
  return e.accept([this](const auto& e){ return codegen(e); });
}

auto Compiler::codegen(const Stmt::Return& s) -> std::expected<void, CodegenError> {
  if (not s.expr) {
    mBuilder->CreateRetVoid();
    return {};
  }

  auto value = TRY(codegen(*s.expr));
  mBuilder->CreateRet(value);
  return {};
}

auto Compiler::codegen(const Stmt::VariableDeclaration& s) -> std::expected<void, CodegenError> {
  const auto type = convertType(s.type);
  // I had a bug here once, when I put `auto` instead of `auto&`. The latter
  // pretty much copies the `Scope` object, instead of referencing it. That's why I got a segfault.
  auto& scope = mSemanticContext.getCurrentScope();

  if (mBuilder->GetInsertBlock() == nullptr) {
    TODO();
  }

  auto* value = TRY(codegen(s.value));
  scope.assignVariable(s.identifier.lexeme, type, value);
  return {};
}

auto Compiler::codegen(const Stmt::Block& s) -> std::expected<void, CodegenError> {
  // I had a bug here once, when I put `auto` instead of `auto&`. The latter
  // pretty much copies the `Scope` object, instead of referencing it. That's why I got a segfault.
  auto& scope = mSemanticContext.createScope();

  for (const auto& statement : s.statements) {
    scope.addContext(&statement);
    auto status = codegen(statement);
    if (not status) {
      return std::unexpected(status.error());
    }
  }

  mSemanticContext.popCurrentScope();
  return {};
}

auto Compiler::codegen(const Stmt::FunctionDeclaration& s) -> std::expected<void, CodegenError> {
  std::vector<llvm::Type*> argsType(s.arguments.size());

  for (const auto& arg : s.arguments) {
    argsType.push_back(convertType(arg.type));
  }

  auto* returnType = convertType(s.returnType);
  auto* functionType = llvm::FunctionType::get(returnType, argsType, false);
  auto* function = llvm::Function::Create(functionType, 
                                          llvm::Function::ExternalLinkage, 
                                          s.identifier.lexeme,
                                          *mModule);

  // NOTE:
  // we do not need to create a scope since a function has `Stmt::Block` as it's body
  // which we create a scope every time we codegen from
  auto& scope = mSemanticContext.getCurrentScope();
  for (auto& arg : function->args()) {
    auto functionArg = s.arguments[arg.getArgNo()];

    auto name = functionArg.identifier.lexeme;
    auto type = functionArg.type;

    arg.setName(name);
    scope.assignVariable(name, convertType(type), &arg);
  }

  auto* block = llvm::BasicBlock::Create(*mContext, "entry", function);
  mBuilder->SetInsertPoint(block);
  return codegen(*s.body);
}

auto Compiler::codegen(const Stmt::Assignment& s) -> std::expected<void, CodegenError> {

  auto variableDeclaration = mSemanticContext.getVariableDeclaration(s.identifier.lexeme);
  if (not variableDeclaration) {
    return std::unexpected(CodegenError::UndefinedVariable(s.identifier.lexeme, s.identifier.location));
  }

  if (not variableDeclaration.value()->isMutable) {
    return std::unexpected(CodegenError::AssignmentToImmutableVariable(s.identifier.lexeme, s.identifier.location));
  }

  auto variable = mSemanticContext.getVariableInstruction(s.identifier.lexeme);
  auto* newValue = TRY(codegen(s.value));
  mBuilder->CreateStore(newValue, *variable);
  return {};
}

auto Compiler::codegen(const Stmt::Control&) -> std::expected<void, CodegenError> {
  TODO();
}

auto Compiler::codegen(const Expr::Binary&) -> std::expected<llvm::Value*, CodegenError> {
  TODO();
}

auto Compiler::codegen(const Expr::Unary&) -> std::expected<llvm::Value*, CodegenError> {
  TODO();
}

auto Compiler::codegen(const Expr::Identifier& e) -> std::expected<llvm::Value*, CodegenError> {
  auto instruction = mSemanticContext.getVariableInstruction(e.identifier.lexeme);
  if (not instruction) {
    return std::unexpected(CodegenError::UndefinedVariable(e.identifier.lexeme, e.identifier.location));
  }
  return mBuilder->CreateLoad(instruction.value()->getAllocatedType(), instruction.value(), e.identifier.lexeme);
}

auto Compiler::codegen(const Expr::Literal& e) -> std::expected<llvm::Value*, CodegenError> {

  auto type = mSemanticContext.getAppropriateExprType();

  if (not type.has_value()) {
    TODO();
  }

  if (type->is<LevType::Inferred>()) {
    switch (e.value.type) {
      case TokenType::Integer:
        return llvm::ConstantInt::get(mBuilder->getInt32Ty(), std::stoi(std::string(e.value.lexeme)));
      case TokenType::Float:
        return llvm::ConstantFP::get(mBuilder->getDoubleTy(), std::stod(std::string(e.value.lexeme)));
      case TokenType::False:
        return mBuilder->getFalse();
      case TokenType::True:
        return mBuilder->getTrue();
      default:
        TODO();
    }
  } 

  if (type->is<LevType::Builtin>()) {
    auto llvmType = convertType(*type);

    switch (e.value.type) {
      case TokenType::Integer:
        return llvm::ConstantInt::get(llvmType, std::stoi(std::string(e.value.lexeme)));
      case TokenType::Float:
        return llvm::ConstantFP::get(llvmType, std::stod(std::string(e.value.lexeme)));
      default:
        TODO();
    }
  }
}

auto Compiler::convertType(const LevType& type) const -> llvm::Type* {
  return type.accept(match {
    [this](const LevType::Builtin& s) -> llvm::Type* {
      switch (s.type) {
        case LevType::Builtin::Types::i8:
        case LevType::Builtin::Types::u8:
          return mBuilder->getInt8Ty();
        case LevType::Builtin::Types::i16:
        case LevType::Builtin::Types::u16:
          return mBuilder->getInt16Ty();
        case LevType::Builtin::Types::i32:
        case LevType::Builtin::Types::u32:
          return mBuilder->getInt32Ty();
        case LevType::Builtin::Types::i64:
        case LevType::Builtin::Types::u64:
          return mBuilder->getInt32Ty();

        case LevType::Builtin::Types::f32:
          return mBuilder->getFloatTy();
        case LevType::Builtin::Types::f64:
          return mBuilder->getDoubleTy();

        case LevType::Builtin::Types::boolean:
          return mBuilder->getInt1Ty();
      }
    },

    [](const auto& s) -> llvm::Type* {
      TODO();
    }
  });
}

