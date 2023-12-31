#include <lev/codegen/compiler.h>
#include <lev/misc/match.h>
#include <lev/misc/macros.h>

#include <utility>

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

auto Compiler::codegen(const Stmt::Control& e) -> std::expected<void, CodegenError> {
  auto* function = mBuilder->GetInsertBlock()->getParent();
  if (function == nullptr) {
    std::unreachable();
  }

  auto* mergeBB = llvm::BasicBlock::Create(*mContext, "ifend", function);
  static const auto codegenBranch = 
    [this, &function, &mergeBB] (const Branch& branch) -> std::expected<llvm::BasicBlock*, CodegenError> {
    auto condition = TRY(codegen(branch.condition));

    condition = mBuilder->CreateICmpEQ(condition, mBuilder->getTrue(), "ifcond");
    
    auto* thenBB = llvm::BasicBlock::Create(*mContext, "then", function);
    auto* elseBB = llvm::BasicBlock::Create(*mContext, "else", function);

    mBuilder->CreateCondBr(condition, thenBB, elseBB);
    mBuilder->SetInsertPoint(thenBB);

    // codegen the then body
    codegen(*branch.body);
    mBuilder->CreateBr(mergeBB);
    mBuilder->SetInsertPoint(elseBB);
    return elseBB;
  };

  auto lastBB = TRY(codegenBranch(e.ifBranch));

  for (auto i = 0; i < e.elseIfBranches.size(); i++) {
    lastBB = TRY(codegenBranch(e.elseIfBranches[i]));
  }

  if (e.elseBody) {
    codegen(**e.elseBody);
    mBuilder->CreateBr(mergeBB);
  }

  mergeBB->moveAfter(lastBB);
  mBuilder->SetInsertPoint(mergeBB);
  return {};
}

auto Compiler::codegen(const Expr::Binary& e) -> std::expected<llvm::Value*, CodegenError> {
  const auto left = TRY(codegen(*e.left));
  const auto right = TRY(codegen(*e.right));

  const auto type = mSemanticContext.getAppropriateExprType();
  if (not type) {
    TODO();
  }

  const auto isFloat = mSemanticContext.typeIsFloat(*type);
  const auto isInteger = mSemanticContext.typeIsInteger(*type);
  const auto isSigned = mSemanticContext.typeIsSigned(*type);

  switch (e.op.type) {
    case TokenType::Plus:
      // TODO: create type is integer/ type is unsigned
      if (isInteger) {
        return mBuilder->CreateAdd(left, right, "addtmp");
      } else if (isFloat) {
        return mBuilder->CreateFAdd(left, right, "addtmp");
      } else {
        std::unreachable();
      }

    case TokenType::Minus:
      if (isInteger) {
        return mBuilder->CreateSub(left, right, "subtmp");
      } else if (isFloat) {
        return mBuilder->CreateFSub(left, right, "subtmp");
      } else {
        std::unreachable();
      }

    case TokenType::Star:
      if (isInteger) {
        return mBuilder->CreateMul(left, right, "multmp");
      } else if (isFloat) {
        return mBuilder->CreateFMul(left, right, "multmp");
      } else {
        std::unreachable();
      }

    case TokenType::Slash:
      if (isInteger) {
        if (isSigned) {
          return mBuilder->CreateSDiv(left, right, "divtmp");
        } else {
          return mBuilder->CreateUDiv(left, right, "divtmp");
        }
      } else if (isFloat) {
        return mBuilder->CreateFDiv(left, right, "divtmp");
      } else {
        std::unreachable();
      }

    case TokenType::Greater: {
      if (isInteger) {
        if (isSigned) {
          return mBuilder->CreateICmpSGT(left, right, "getmp");
        } else {
          return mBuilder->CreateICmpUGT(left, right, "getmp");
        }
      } else if (isFloat) {
        return mBuilder->CreateFCmpUGT(left, right, "getmp");
      } else {
        std::unreachable();
      }
    }
    case TokenType::GreaterEqual: {
      if (isInteger) {
        if (isSigned) {
          return mBuilder->CreateICmpSGE(left, right, "getmp");
        } else {
          return mBuilder->CreateICmpUGE(left, right, "getmp");
        }
      } else if (isFloat) {
        return mBuilder->CreateFCmpUEQ(left, right, "getmp");
      } else {
        std::unreachable();
      }
    }
    case TokenType::Less: {
      if (isInteger) {
        if (isSigned) {
          return mBuilder->CreateICmpSLT(left, right, "ltmp");
        } else {
          return mBuilder->CreateICmpULT(left, right, "ltmp");
        }
      } else if (isFloat) {
        return mBuilder->CreateFCmpULT(left, right, "ltmp");
      } else {
        std::unreachable();
      }
    }
    case TokenType::LessEqual: {
      if (isInteger) {
        if (isSigned) {
          return mBuilder->CreateICmpSLE(left, right, "letmp");
        } else {
          return mBuilder->CreateICmpULE(left, right, "letmp");
        }
      } else if (isFloat) {
        return mBuilder->CreateFCmpULE(left, right, "letmp");
      } else {
        std::unreachable();
      }
    }
    case TokenType::BangEqual:
      if (isInteger) {
        if (isSigned) {
          return mBuilder->CreateICmpNE(left, right, "netmp");
        } else {
          return mBuilder->CreateICmpNE(left, right, "netmp");
        }
      } else if (isFloat) {
        return mBuilder->CreateFCmpUNE(left, right, "netmp");
      } else {
        std::unreachable();
      }
    case TokenType::EqualEqual:
      if (isInteger) {
        return mBuilder->CreateICmpEQ(left, right, "eetmp");
      } else if (isFloat) {
        return mBuilder->CreateICmpEQ(left, right, "eetmp");
      } else {
        std::unreachable();
      }
    default:
      std::unreachable();
  }
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

  TODO();
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

