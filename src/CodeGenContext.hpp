#pragma once
#include "AST.hpp"

extern llvm::LLVMContext Context;
extern llvm::IRBuilder<> Builder;

class CodeGenContext {
    public:
        llvm::Module* module;
        CodeGenContext();
        llvm::TypeSize getTypeSize(llvm::Type* type);
        void AddFunc(std::string name, llvm::Function* Func);
        llvm::Function* GetFunc(std::string name);
        void EnterFunc(llvm::Function* Func);
        void ExitFunc();
        llvm::Function* GetCurrFunc();
        void SetInsertPoint(llvm::BasicBlock* bBlock);
        llvm::BasicBlock* GetInsertPoint();
        llvm::AllocaInst* CreateEntryBlockAlloca(llvm::Function* function, std::string varName);
        llvm::Value* GetVariable(std::string name);
        void AddVar(std::string name, llvm::Value* val);
        void GenerateIR(AST::Program& ast);
        ~CodeGenContext();
        // ~CodeGenContext() = default;
    private:
        llvm::Function* CurrFunc;
        llvm::BasicBlock* InsertPoint;
        std::map<std::string, llvm::Value*> Variables;
        std::map<std::string, llvm::Function*> Funcs;
};