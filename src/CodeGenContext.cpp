#include "CodeGenContext.hpp"
#include "AST.hpp"

llvm::LLVMContext Context;
llvm::IRBuilder<> Builder(Context);

CodeGenContext::CodeGenContext() : module(new llvm::Module("main", Context)) {}

llvm::TypeSize CodeGenContext::getTypeSize(llvm::Type* type){
    return module->getDataLayout().getTypeSizeInBits(type);
}

void CodeGenContext::AddFunc(std::string name, llvm::Function* Func){
    this->Funcs[name] = Func;
}

llvm::Function* CodeGenContext::GetFunc(std::string name){
    return this->Funcs[name];
}

void CodeGenContext::EnterFunc(llvm::Function* Func){
    this->CurrFunc = Func;
}

void CodeGenContext::ExitFunc(){
    this->CurrFunc = NULL;
}

llvm::Function* CodeGenContext::GetCurrFunc(){
    return this->CurrFunc;
}

void CodeGenContext::SetInsertPoint(llvm::BasicBlock* bBlock){
    this->InsertPoint = bBlock;
}

llvm::BasicBlock* CodeGenContext::GetInsertPoint(){
    return this->InsertPoint;
}

llvm::AllocaInst* CodeGenContext::CreateEntryBlockAlloca(llvm::Function* function, std::string varName){
    llvm::IRBuilder<> TmpB(&function->getEntryBlock(), function->getEntryBlock().begin());
    return TmpB.CreateAlloca(llvm::Type::getInt32Ty(Context), 0, varName.c_str());
}

llvm::Value* CodeGenContext::GetVariable(std::string name){
    return this->Variables[name];
}

void CodeGenContext::AddVar(std::string name, llvm::Value* val){
    this->Variables[name] = val;
}

void CodeGenContext::GenerateIR(AST::Program& ast){
    ast.CodeGen(*this);
}

CodeGenContext::~CodeGenContext(){
    delete this->module;
}