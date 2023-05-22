#include "CodeGenContext.hpp"

CodeGenContext::CodeGenContext()
    : _module(new llvm::Module("main", _llvmContext)),
      _builder(*new llvm::IRBuilder<>(_llvmContext)),
      _symbolTableStack(1),
      _functionTable(std::unordered_map<std::string, llvm::Function*>()) {
}

llvm::LLVMContext& CodeGenContext::getLLVMContext() {
    return _llvmContext;
}

llvm::Module* CodeGenContext::getModule() {
    return _module;
}

llvm::IRBuilder<>& CodeGenContext::builder() {
    return _builder;
}

void CodeGenContext::pushSymbolTable() {
    _symbolTableStack.push_back(std::unordered_map<std::string, llvm::Value*>());
}

void CodeGenContext::popSymbolTable() {
    _symbolTableStack.pop_back();
}

bool CodeGenContext::isGlobalScope() {
    return _symbolTableStack.size() == 1;
}

void CodeGenContext::createSymbolTableEntry(const std::string& name, llvm::Value* value) {
    _symbolTableStack.back()[name] = value;
}

void CodeGenContext::createSymbolTableEntry(const std::string& name, llvm::Type* type) {
    void* value = static_cast<void*>(type);
    // link type alias with type and create a new entry in symbol table
    _symbolTableStack.back()[name] = reinterpret_cast<llvm::Value*>(value);
}

llvm::Value* CodeGenContext::getSymbolTableEntry(const std::string& name) {
    for (auto it = _symbolTableStack.rbegin(); it != _symbolTableStack.rend(); ++it) {
        auto entry = it->find(name);
        if (entry != it->end()) {
            return entry->second;
        }
    }
    return nullptr;
}

llvm::AllocaInst* CodeGenContext::createEntryBlockAlloca(llvm::Type* type, const std::string& name) {
    llvm::Function* function = builder().GetInsertBlock()->getParent();
    llvm::IRBuilder<> tmpBuilder(&function->getEntryBlock(), function->getEntryBlock().begin());
    return tmpBuilder.CreateAlloca(type, 0, name);
}

unsigned int CodeGenContext::getStructMemberIndex(llvm::StructType* structType, const std::string& memberName) {
    //traverse struct members and find the member of the given name
    for (unsigned int i = 0; i < structType->getNumElements(); i++) {
        llvm::StringRef memberNameRef = structType->getElementType(i)->getStructName();
        if (memberNameRef == memberName) {
            return i;   //return index of the member
        }
    }
    return -1;  //return -1 if member not found
}

void CodeGenContext::AddFunc(const std::string& functionName, llvm::Function* function) {
    //add function to function table
    _functionTable[functionName] = function;
}


llvm::Function* CodeGenContext::getFunction(const std::string& name) {
    //search for function in function table
    auto it = _functionTable.find(name);
    if (it != _functionTable.end()) {
        return it->second;
    }

    //if not found, return NULL
    return NULL;
}