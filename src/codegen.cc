#include "codegen.h"

using namespace lev::codegen;
using namespace llvm;

using Codegen = lev::codegen::Codegen;

Codegen::Codegen() {
  mContext = std::make_unique<LLVMContext>();
  mModule = std::make_unique<Module>("lev", *mContext);
  mBuilder = std::make_unique<IRBuilder<>>(*mContext);
}

auto Codegen::convertType(Type type) -> llvm::Type* {
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

auto Codegen::visit(FunctionDeclaration& f) -> void {
  llvm::Type* type = convertType(f.returnType);

  auto* funcType = FunctionType::get(type, false);
  auto* func = Function::Create(funcType, llvm::Function::ExternalLinkage, f.functionName, *mModule);
  auto* block = BasicBlock::Create(*mContext, "entry", func);

  mBuilder->SetInsertPoint(block);
  mBuilder->CreateRet(mBuilder->getInt8(0));
}

auto Codegen::visit(VariableDeclaration& v) -> void {
  if (mCurrentFunction == nullptr) {
    mModule->getOrInsertGlobal(v.identifier.lexeme, convertType(v.type));
    auto* gVar = mModule->getNamedGlobal(v.identifier.lexeme);
    gVar->setLinkage(GlobalValue::CommonLinkage);
    gVar->setAlignment(Align(4));
    return;
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
