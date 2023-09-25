#include "codegen.h"
#include "parser.h"
#include "scanner.h"

#include <ranges>
#include <variant>
#include <string>
#include <print>

// https://en.cppreference.com/w/cpp/utility/variant/codegen
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

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
    [](const Unimplemented&){ 
      std::println("Unimplemented error");
    },
    [](const UndefinedVariable&){ 
      std::println("Undefined variable error");
    },
    [](const InvalidUnaryType&){ 
      std::println("Invalid unary error");
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

  for (auto& arg : function->args()) {
    const auto name = f.args[arg.getArgNo()].first;
    arg.setName(name);
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

    const auto value = codegenExpr(*v.value);
    if (not value) {
      return std::unexpected(value.error());
    }
    globalVariable->setInitializer((Constant*) *value);
    return true;
  }

  auto* alloca = mBuilder->CreateAlloca(type, nullptr, v.identifier.lexeme);
  mNamedValues[std::string(v.identifier.lexeme)] = alloca;

  const auto key = std::string(v.identifier.lexeme);
  const auto value = codegenExpr(*v.value);

  if (not value) {
    return std::unexpected(value.error());
  }
  mBuilder->CreateStore(*value, alloca);
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
  if (not mNamedValues.contains(key)) {
    return std::unexpected(UndefinedVariable{});
  }
  return mNamedValues.at(key);
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

auto Codegen::codegen(const UnaryExpr& e) -> std::expected<llvm::Value*, CodegenError> {
  return std::unexpected(Unimplemented{});
}

auto Codegen::codegen(const CallExpr& e) -> std::expected<llvm::Value*, CodegenError> {
  return std::unexpected(Unimplemented{});
}

auto Codegen::codegen(const AssignStmt& e) -> std::expected<bool, CodegenError> {
  return std::unexpected(Unimplemented{});
}

auto Codegen::codegen(const BlockStmt& e) -> std::expected<bool, CodegenError> {
  for (const auto& statement : e.statements) {
    codegenStmt(statement);
  }
  return true;
}

