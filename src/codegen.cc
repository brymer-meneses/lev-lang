#include "codegen.h"
#include "parser.h"
#include "scanner.h"

#include <variant>
#include <string>

// https://en.cppreference.com/w/cpp/utility/variant/visit
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

using namespace lev::codegen;
using namespace lev::parser;
using namespace lev::ast;

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
    if (not codegen(statement)) {
      std::cerr << "Encountered error\n";
      break;
    }
  }
}

auto Codegen::dump() const -> std::string {
  std::string str;
  llvm::raw_string_ostream OS(str);

  OS << *mModule;
  OS.flush();
  return str;
}

auto Codegen::codegen(const Stmt& stmt) -> std::expected<bool, CodegenError> {
  return stmt.accept([this](const auto &e) { return visit(e); });
}

auto Codegen::codegen(const Expr& expr) -> std::expected<llvm::Value*, CodegenError> {
  return expr.accept([this](const auto &e) { return visit(e); });
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

static auto getDefaultType(TokenType type) -> std::optional<lev::ast::Type> {
  switch (type) {
    case TokenType::Integer:
      return lev::ast::Type::i32;
    case TokenType::Float:
      return lev::ast::Type::f32;
    case TokenType::True:
    case TokenType::False:
      return lev::ast::Type::Bool;
    case TokenType::String:
      return lev::ast::Type::String;
    default:
      return std::nullopt;
  }
} 
auto Codegen::inspectVariableExprType(const Expr::VariableExpr& e) const -> std::expected<ast::Type, CodegenError> {
  // TODO: we should look at the variable name and report its type
  return std::unexpected(Unimplemented{});
}

auto Codegen::inspectBinaryExprType(const Expr::BinaryExpr& e) const -> std::expected<ast::Type, CodegenError> {
  const auto leftType = inspectExprType(*e.left);
  const auto rightType = inspectExprType(*e.right);
  // TODO: this is ugly :<
  if (leftType == Type::i32 and rightType == Type::i32) {
    return Type::i32;
  }
  if (leftType == Type::f32 and rightType == Type::f32) {
    return Type::f32;
  }
  if (leftType == Type::i32 and rightType == Type::f32) {
    return Type::f32;
  }
  if (leftType == Type::f32 and rightType == Type::i32) {
    return Type::f32;
  }
  return std::unexpected(Unimplemented{});
}

auto Codegen::inspectLiteralExprType(const Expr::LiteralExpr& e) const -> std::expected<ast::Type, CodegenError> {
  // TODO: we really should check for the expected type of the current stmt
  auto type = getDefaultType(e.value.type);
  if (not type) {
    return std::unexpected(Unimplemented{});
  }
  return *type;
}

auto Codegen::inspectUnaryExprType(const Expr::UnaryExpr& e) const -> std::expected<ast::Type, CodegenError> {
  if (e.op.type == TokenType::Bang) {
    if (inspectExprType(*e.right) == Type::Bool) {
      return Type::Bool;
    } else {
      return std::unexpected(InvalidUnaryType{});
    }
  }
  return std::unexpected(Unimplemented{});
}

auto Codegen::inspectExprType(const Expr& expr) const -> std::expected<lev::ast::Type, CodegenError> {
  using lev::ast::Type;
  static const auto visitor = overloaded {
    [this](const Expr::LiteralExpr& e) {
      return inspectLiteralExprType(e);
    },

    [this](const Expr::UnaryExpr& e) {
      return inspectUnaryExprType(e);
    },

    [this](const Expr::BinaryExpr& e) {
      return inspectBinaryExprType(e);
    },
    [this](const Expr::VariableExpr& e) {
      return inspectVariableExprType(e);
    }
  };
  return expr.accept(visitor);
}

auto Codegen::visit(const Stmt::ExprStmt& e) -> std::expected<bool, CodegenError> {
  return std::unexpected(Unimplemented{});
}

auto Codegen::visit(const Stmt::FunctionDeclarationStmt& f) -> std::expected<bool, CodegenError> {
  std::vector<llvm::Type*> argsType(f.args.size());
  for (const auto& arg : f.args) {
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
  return true;
}

auto Codegen::visit(const Stmt::VariableDeclarationStmt& v) -> std::expected<bool, CodegenError> {
  const auto type = convertType(v.type);

  if (mBuilder->GetInsertBlock() == nullptr) {
    mModule->getOrInsertGlobal(v.identifier.lexeme, convertType(v.type));
    auto* globalVariable = mModule->getNamedGlobal(v.identifier.lexeme);
    globalVariable->setLinkage(GlobalValue::CommonLinkage);
    globalVariable->setAlignment(Align(4));
    globalVariable->setConstant(not v.isMutable);

    auto value = codegen(*v.value);
    if (not value) {
      return std::unexpected(value.error());
    }
    globalVariable->setInitializer((Constant*) *value);
    return true;
  }

  auto* alloca = mBuilder->CreateAlloca(type, nullptr, v.identifier.lexeme);
  auto value = codegen(*v.value);
  if (not value) {
    return std::unexpected(value.error());
  }
  mBuilder->CreateStore(*value, alloca);
  return true;
}

auto Codegen::visit(const Expr::LiteralExpr& e) -> std::expected<llvm::Value*, CodegenError> {
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
      return std::unexpected(Unimplemented{});
  }
};

auto Codegen::visit(const Expr::VariableExpr& e) -> std::expected<llvm::Value*, CodegenError> {
  return nullptr;
}

auto Codegen::visit(const Expr::BinaryExpr& e) -> std::expected<llvm::Value*, CodegenError> {
  auto left = codegen(*e.left);
  auto right = codegen(*e.right);

  if (not left) {
    return left;
  }
  if (not right) {
    return right;
  }

  auto type = inspectBinaryExprType(e);

  switch (e.op.type) {
    case TokenType::Plus:
      // TODO: create type is integer/ type is unsigned
      if (type == ast::Type::i32) {
        return mBuilder->CreateAdd(*left, *right, "addtmpsi");
      } else if (type == ast::Type::f32) {
        return mBuilder->CreateFAdd(*left, *right, "addtmpf");
      }
    case TokenType::Minus:
      if (type == ast::Type::i32) {
        return mBuilder->CreateSub(*left, *right, "subtmpsi");
      } else if (type == ast::Type::f32) {
        return mBuilder->CreateFSub(*left, *right, "subtmpf");
      }
    case TokenType::Star:
      if (type == ast::Type::i32) {
        return mBuilder->CreateMul(*left, *right, "multmpsi");
      } else if (type == ast::Type::f32) {
        return mBuilder->CreateFMul(*left, *right, "multmpf");
      }
    case TokenType::Slash:
      if (type == ast::Type::i32) {
        return mBuilder->CreateSDiv(*left, *right, "divtmpsi");
      } else if (type == ast::Type::f32) {
        return mBuilder->CreateFDiv(*left, *right, "divtmpf");
      }

    case TokenType::Greater:
    case TokenType::GreaterEqual:
    case TokenType::Less:
    case TokenType::LessEqual:
    case TokenType::Bang:
    case TokenType::BangEqual:
    // TODO: implement this
    default:
      return std::unexpected(Unimplemented{});
  }

  return std::unexpected(Unimplemented{});
}

auto Codegen::visit(const Expr::UnaryExpr& e) -> std::expected<llvm::Value*, CodegenError> {
  return std::unexpected(Unimplemented{});
}

auto Codegen::visit(const Stmt::AssignStmt& e) -> std::expected<bool, CodegenError> {
  return std::unexpected(Unimplemented{});
}

auto Codegen::visit(const Stmt::BlockStmt& e) -> std::expected<bool, CodegenError> {
  for (const auto& statement : e.statements) {
    codegen(statement);
  }
  return true;
}

