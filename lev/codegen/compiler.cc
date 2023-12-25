#include "misc/match.h"
#include <lev/codegen/compiler.h>
#include <lev/misc/macros.h>

using namespace lev;

Compiler::Compiler(std::vector<Stmt> statements) {
  mContext = std::make_unique<llvm::LLVMContext>();
  mModule = std::make_unique<llvm::Module>("lev", *mContext);
  mBuilder = std::make_unique<llvm::IRBuilder<>>(*mContext);
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

  if (mBuilder->GetInsertBlock() == nullptr) {
    TODO();
  }

  auto* variable = mBuilder->CreateAlloca(type, nullptr, s.identifier.lexeme);
  auto* value = TRY(codegen(s.value));
  auto scope = mSemanticContext.getCurrentScope();

  mBuilder->CreateStore(value, variable);
  scope.defineVariable(s.identifier.lexeme, variable);
  return {};
}

auto Compiler::codegen(const Stmt::Block& s) -> std::expected<void, CodegenError> {
  auto scope = mSemanticContext.createScope();

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

    auto* variable = mBuilder->CreateAlloca(convertType(type), nullptr, name);

    mBuilder->CreateStore(&arg, variable);
    scope.defineVariable(name, variable);
  }

  auto* block = llvm::BasicBlock::Create(*mContext, "entry", function);
  mBuilder->SetInsertPoint(block);
  return codegen(*s.body);
}

auto Compiler::codegen(const Stmt::Assignment& s) -> std::expected<void, CodegenError> {
  auto scope = mSemanticContext.getCurrentScope();
  auto variable = scope.readVariable(s.identifier.lexeme);

  if (not variable) {
    return std::unexpected(CodegenError{});
  }

  auto* newValue = TRY(codegen(s.value));
  mBuilder->CreateStore(newValue, *variable);
  return {};
}

auto Compiler::codegen(const Stmt::Control&) -> std::expected<void, CodegenError> {

}

auto Compiler::codegen(const Expr::Binary&) -> std::expected<llvm::Value*, CodegenError> {

}

auto Compiler::codegen(const Expr::Unary&) -> std::expected<llvm::Value*, CodegenError> {

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
  } else if (type->is<LevType::Builtin>()) {
      TODO();
  }
}

auto Compiler::convertType(const LevType& type) const -> llvm::Type* {
  type.accept(match {
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

