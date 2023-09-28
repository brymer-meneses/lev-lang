#include "codegen.h"
#include "parser.h"
#include "scanner.h"
#include "utils.h"

#include <ranges>
#include <variant>
#include <string>
#include <print>

using namespace lev::codegen;
using namespace lev::parser;
using namespace lev::ast;

using namespace llvm;

using Codegen = lev::codegen::Codegen;

Codegen::Codegen(std::string_view source)  {
  mContext = std::make_unique<LLVMContext>();
  mModule = std::make_unique<Module>("lev", *mContext);
  mBuilder = std::make_unique<IRBuilder<>>(*mContext);

  Parser parser(source);
  auto statements = parser.parse();
  if (not statements) {
    Parser::printError(statements.error());
    return;
  }

  mSemanticContext = SemanticContext(std::move(*statements));
}

Codegen::Codegen(std::vector<Stmt> statements) : mSemanticContext(std::move(statements)) {
  mContext = std::make_unique<LLVMContext>();
  mModule = std::make_unique<Module>("lev", *mContext);
  mBuilder = std::make_unique<IRBuilder<>>(*mContext);
};

auto Codegen::compile() -> void {
  for (const auto& statement : mSemanticContext.statements) {
    codegenStmt(statement);
  }
}

auto Codegen::dump() const -> std::string {
  std::string str;
  llvm::raw_string_ostream OS(str);

  OS << *mModule;
  OS.flush();
  return str;
}

auto Codegen::reportErrors(CodegenError error) -> void {
  std::visit(overloaded {
    [](const Unimplemented& e){ 
      std::println("Unimplemented error {}", e.location.function_name());
    },
    [](const UndefinedVariable&){ 
      std::println("Undefined variable error");
    },
    [](const InvalidUnaryType&){ 
      std::println("Invalid unary error");
    },
    [](const IllFormed& e){ 
      std::println("Illformed error {} at {}", e.location.function_name(), e.location.line());
    }
  }, error);
}

auto Codegen::codegenStmt(const Stmt& stmt) -> std::expected<bool, CodegenError> {
  mSemanticContext.setCurrentStmt(&stmt);
  const auto value =  stmt.accept([this](const auto &e) { return codegen(e); });
  if (not value) {
    reportErrors(value.error());
    exit(0);
  }
  return value;
}

auto Codegen::codegenExpr(const Expr& expr) -> std::expected<llvm::Value*, CodegenError> {
  return expr.accept([this](const auto &e) { return codegen(e); });
}

auto Codegen::convertType(ast::Type type) const -> llvm::Type* {
  switch (type) {
    case Type::i8:
    case Type::u8:
      return mBuilder->getInt8Ty();
    case Type::i16:
    case Type::u16:
      return mBuilder->getInt16Ty();
    case Type::i32:
    case Type::u32:
      return mBuilder->getInt32Ty();
    case Type::i64:
    case Type::u64:
      return mBuilder->getInt64Ty();
    case Type::f32:
      return mBuilder->getFloatTy();
    case Type::f64:
      return mBuilder->getDoubleTy();
    case Type::Bool:
      return mBuilder->getInt1Ty();
    default:
      return mBuilder->getVoidTy();
  };
}


auto Codegen::codegen(const ExprStmt& e) -> std::expected<bool, CodegenError> {
  return std::unexpected(Unimplemented{});
}

auto Codegen::codegen(const FunctionDeclaration& f) -> std::expected<bool, CodegenError> {
  std::vector<llvm::Type*> argsType(f.args.size());
  for (const auto& arg : f.args) {
    const auto type = convertType(arg.second);
    argsType.push_back(type);
  }

  auto* returnType = convertType(f.returnType);

  auto* funcType = FunctionType::get(returnType, argsType, false);
  auto* function = Function::Create(funcType, llvm::Function::ExternalLinkage, f.functionName, *mModule);

  mFunctionStack.clear();
  for (auto& arg : function->args()) {
    auto [name, type] = f.args[arg.getArgNo()];
    arg.setName(name);

    auto alloca = mBuilder->CreateAlloca(convertType(type), nullptr, name);

    mBuilder->CreateStore(&arg, alloca);
    mFunctionStack[std::string(name)] = alloca;
  }


  auto* block = BasicBlock::Create(*mContext, "entry", function);
  mBuilder->SetInsertPoint(block);

  codegenStmt(*f.body);
  return true;
}

auto Codegen::codegen(const VariableDeclaration& v) -> std::expected<bool, CodegenError> {
  const auto type = convertType(v.type);

  if (mBuilder->GetInsertBlock() == nullptr) {
    mModule->getOrInsertGlobal(v.identifier.lexeme, convertType(v.type));
    auto* globalVariable = mModule->getNamedGlobal(v.identifier.lexeme);
    globalVariable->setLinkage(GlobalValue::CommonLinkage);
    globalVariable->setAlignment(Align(4));
    globalVariable->setConstant(not v.isMutable);

    auto value = codegenExpr(*v.value);
    if (not value) {
      return std::unexpected(value.error());
    }
    globalVariable->setInitializer((Constant*) *value);
    return true;
  }

  auto* alloca = mBuilder->CreateAlloca(type, nullptr, v.identifier.lexeme);

  const auto key = std::string(v.identifier.lexeme);
  const auto value = codegenExpr(*v.value);

  if (not value) {
    return std::unexpected(value.error());
  }
  mBuilder->CreateStore(*value, alloca);
  mFunctionStack[key] = alloca;
  return true;
}

auto Codegen::codegen(const LiteralExpr& e) -> std::expected<llvm::Value*, CodegenError> {
  switch (e.value.type) {
    case TokenType::Integer:
      return ConstantInt::get(convertType(Type::i32), std::stoi(std::string(e.value.lexeme)));
    case TokenType::Float:
      return ConstantFP::get(convertType(Type::f32), std::stod(std::string(e.value.lexeme)));
    case TokenType::False:
      return ConstantInt::get(convertType(Type::Bool), 0);
    case TokenType::True:
      return ConstantInt::get(convertType(Type::Bool), 1);
    default:
      return std::unexpected(Unimplemented{});
  }
};

auto Codegen::codegen(const VariableExpr& e) -> std::expected<llvm::Value*, CodegenError> {
  const auto key = std::string(e.identifier.lexeme);
  if (not mFunctionStack.contains(key)) {
    return std::unexpected(UndefinedVariable{});
  }

  auto* variable = mFunctionStack.at(key);
  return mBuilder->CreateLoad(variable->getAllocatedType(), variable, e.identifier.lexeme);
}

auto Codegen::codegen(const BinaryExpr& e) -> std::expected<llvm::Value*, CodegenError> {
  const auto left = codegenExpr(*e.left);
  const auto right = codegenExpr(*e.right);

  if (not left) {
    return left;
  }
  if (not right) {
    return right;
  }

  const auto type = mSemanticContext.inferType(e);
  if (not type) {
    mSemanticContext.reportError(type.error());
    exit(1);
  }

  const auto isFloat = mSemanticContext.isFloat(*type);
  const auto isInteger = mSemanticContext.isInteger(*type);
  const auto isSigned = mSemanticContext.isSigned(*type);

  switch (e.op.type) {
    case TokenType::Plus:
      // TODO: create type is integer/ type is unsigned
      if (isInteger) {
        return mBuilder->CreateAdd(*left, *right, "addtmp");
      } else if (isFloat) {
        return mBuilder->CreateFAdd(*left, *right, "addtmp");
      } else {
        return std::unexpected(IllFormed{});
      }

    case TokenType::Minus:
      if (isInteger) {
        return mBuilder->CreateSub(*left, *right, "subtmp");
      } else if (isFloat) {
        return mBuilder->CreateFSub(*left, *right, "subtmp");
      } else {
        return std::unexpected(IllFormed{});
      }

    case TokenType::Star:
      if (isInteger) {
        return mBuilder->CreateMul(*left, *right, "multmp");
      } else if (isFloat) {
        return mBuilder->CreateFMul(*left, *right, "multmp");
      } else {
        return std::unexpected(IllFormed{});
      }

    case TokenType::Slash:
      if (isInteger) {
        if (isSigned) {
          return mBuilder->CreateSDiv(*left, *right, "divtmp");
        } else {
          return mBuilder->CreateUDiv(*left, *right, "divtmp");
        }
      } else if (isFloat) {
        return mBuilder->CreateFDiv(*left, *right, "divtmp");
      } else {
        return std::unexpected(IllFormed{});
      }

    case TokenType::Greater: {
      if (isInteger) {
        if (isSigned) {
          return mBuilder->CreateICmpSGT(*left, *right, "getmp");
        } else {
          return mBuilder->CreateICmpUGT(*left, *right, "getmp");
        }
      } else if (isFloat) {
        return mBuilder->CreateFCmpUGT(*left, *right, "getmp");
      } else {
        return std::unexpected(IllFormed{});
      }
    }
    case TokenType::GreaterEqual: {
      if (isInteger) {
        if (isSigned) {
          return mBuilder->CreateICmpSGE(*left, *right, "getmp");
        } else {
          return mBuilder->CreateICmpUGE(*left, *right, "getmp");
        }
      } else if (isFloat) {
        return mBuilder->CreateFCmpUEQ(*left, *right, "getmp");
      } else {
        return std::unexpected(IllFormed{});
      }
    }
    case TokenType::Less: {
      if (isInteger) {
        if (isSigned) {
          return mBuilder->CreateICmpSLT(*left, *right, "ltmp");
        } else {
          return mBuilder->CreateICmpULT(*left, *right, "ltmp");
        }
      } else if (isFloat) {
        return mBuilder->CreateFCmpULT(*left, *right, "ltmp");
      } else {
        return std::unexpected(IllFormed{});
      }
    }
    case TokenType::LessEqual: {
      if (isInteger) {
        if (isSigned) {
          return mBuilder->CreateICmpSLE(*left, *right, "letmp");
        } else {
          return mBuilder->CreateICmpULE(*left, *right, "letmp");
        }
      } else if (isFloat) {
        return mBuilder->CreateFCmpULE(*left, *right, "letmp");
      } else {
        return std::unexpected(IllFormed{});
      }
    }
    case TokenType::BangEqual:
      if (isInteger) {
        if (isSigned) {
          return mBuilder->CreateICmpNE(*left, *right, "netmp");
        } else {
          return mBuilder->CreateICmpNE(*left, *right, "netmp");
        }
      } else if (isFloat) {
        return mBuilder->CreateFCmpUNE(*left, *right, "netmp");
      } else {
        return std::unexpected(IllFormed{});
      }
    case TokenType::EqualEqual:
      if (isInteger) {
        return mBuilder->CreateICmpEQ(*left, *right, "eetmp");
      } else if (isFloat) {
        return mBuilder->CreateICmpEQ(*left, *right, "eetmp");
      } else {
        return std::unexpected(IllFormed{});
      }
      
    default:
      return std::unexpected(Unimplemented{});
  }

  return std::unexpected(Unimplemented{});
}

auto Codegen::codegen(const UnaryExpr& e) -> std::expected<llvm::Value*, CodegenError> {
  return std::unexpected(Unimplemented{});
}

auto Codegen::codegen(const CallExpr& e) -> std::expected<llvm::Value*, CodegenError> {
  return std::unexpected(Unimplemented{});
}

auto Codegen::codegen(const AssignStmt& e) -> std::expected<bool, CodegenError> {
  // TODO: check if the variable is mutable first
  const auto key = std::string(e.identifier.lexeme);
  if (not mFunctionStack.contains(key)) {
    return std::unexpected(UndefinedVariable{});
  }
  auto* variable = mFunctionStack.at(key);
  auto newValue = codegenExpr(*e.value);
  if (not newValue) {
    return std::unexpected(newValue.error());
  }
  mBuilder->CreateStore(*newValue, variable);
  return true;
}

auto Codegen::codegen(const BlockStmt& e) -> std::expected<bool, CodegenError> {
  for (const auto& statement : e.statements) {
    codegenStmt(statement);
  }
  return true;
}

auto Codegen::codegen(const ReturnStmt& s) -> std::expected<bool, CodegenError> {

  if (not s.expr) {
    mBuilder->CreateRetVoid();
    return true;
  }

  auto value = codegenExpr(*s.expr);
  if (not value) {
    return std::unexpected(IllFormed{});
  }

  mBuilder->CreateRet(*value);
  return true;
}

auto Codegen::codegen(const IfStmt& e) -> std::expected<bool, CodegenError> {

  auto* function = mBuilder->GetInsertBlock()->getParent();
  if (function == nullptr) {
    return std::unexpected(IllFormed{});
  }

  auto* mergeBB = BasicBlock::Create(*mContext, "ifend", function);

  static const auto codegenBranch = 
    [this, &function, &mergeBB] (const IfStmt::Branch& branch) -> std::expected<BasicBlock*, CodegenError> {
    auto condition = codegenExpr(branch.condition);
    if (not condition) {
      return std::unexpected(condition.error());
    }

    condition = mBuilder->CreateICmpEQ(*condition, mBuilder->getTrue(), "ifcond");
    
    auto* thenBB = BasicBlock::Create(*mContext, "then", function);
    auto* elseBB = BasicBlock::Create(*mContext, "else", function);

    mBuilder->CreateCondBr(*condition, thenBB, elseBB);
    mBuilder->SetInsertPoint(thenBB);

    // codegen the then body
    codegenStmt(*branch.body);
    mBuilder->CreateBr(mergeBB);
    mBuilder->SetInsertPoint(elseBB);

    return elseBB;
  };

  auto lastBB = codegenBranch(e.ifBranch);
  if (not lastBB) {
    return std::unexpected(lastBB.error());
  }

  for (auto i = 0; i < e.elseIfBranches.size(); i++) {
    lastBB = codegenBranch(e.elseIfBranches[i]);
    if (not lastBB) {
      return std::unexpected(lastBB.error());
    }
  }

  if (e.elseBody) {
    codegenStmt(**e.elseBody);
    mBuilder->CreateBr(mergeBB);
  }

  mergeBB->moveAfter(*lastBB);
  mBuilder->SetInsertPoint(mergeBB);

  return true;
}
