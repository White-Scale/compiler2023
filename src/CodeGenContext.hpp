#pragma once
#include "AST.hpp"
#include <unordered_map>
#include <vector>
#include <string>

class CodeGenContext {
public:
    bool _is_save;
    llvm::LLVMContext _llvmContext;
    llvm::Module* _module;
    llvm::IRBuilder<>& _builder;
    std::unordered_map<std::string, llvm::Value*> _symbolTable;
    std::vector<std::unordered_map<std::string, llvm::Value*>> _symbolTableStack;
    std::unordered_map<std::string, llvm::Function*> _functionTable;

public:
    CodeGenContext();
    ~CodeGenContext();

    llvm::LLVMContext& getLLVMContext();
    llvm::Module* getModule();
    llvm::IRBuilder<>& builder();

    void pushSymbolTable();
    void popSymbolTable();
    bool isGlobalScope();
    void createSymbolTableEntry(const std::string& name, llvm::Value* value);
    void createSymbolTableEntry(const std::string& name, llvm::Type* type);
    llvm::Value* getSymbolTableEntry(const std::string& name);
    llvm::AllocaInst* createEntryBlockAlloca(llvm::Type* type, const std::string& name);
    unsigned int getStructMemberIndex(llvm::StructType* structType, const std::string& memberName);
    void AddFunc(const std::string& functionName, llvm::Function* function);
    llvm::Function* getFunction(const std::string& name);
};