#include"CodeGenContext.hpp"
#include"AST.hpp"

namespace AST{
    //Program
    llvm::Value* Program::CodeGen(CodeGenContext& context){
        for(auto& dec : *(this->_decs)){
            dec->CodeGen(context);
            // dec->CodeGen(context)->print(llvm::outs());
        }
        return NULL;
    }

    //Function Declaration
    llvm::Value* FunDec::CodeGen(CodeGenContext& context){
        std::vector<llvm::Type*> argTypes;
        //argument type list
        if(_args != NULL){
            for(auto arg : *(this->_args)){
                argTypes.push_back(arg->_type->GetType(context));
            }
        }
        //get function type
        llvm::FunctionType* funcType = llvm::FunctionType::get(this->_returnType->GetType(context), argTypes, false);
        //create function
        llvm::Function* func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, this->_name, context.getModule());
        context.AddFunc(this->_name, func);
        llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(context.getLLVMContext(), "entry", func);
        context.builder().SetInsertPoint(entryBlock);
        //Create a new symbol table for this function
        unsigned int index = 0;
        for(llvm::Argument& argument : func->args()){
            Arg* arg = (*_args)[index];
            argument.setName(arg->_name);
            llvm::AllocaInst* alloca = context.createEntryBlockAlloca(arg->_type->GetType(context), arg->_name);
            context.builder().CreateStore(&argument, alloca);
            context.createSymbolTableEntry(arg->_name, alloca);
            index++;
        }
        //Generate code for function body
        this->_body->CodeGen(context);
        //add return statement if not exist
        if(!entryBlock->getTerminator()) {
            if(_returnType->GetType(context) == llvm::Type::getVoidTy(context.getLLVMContext())){
                //return void
                context.builder().CreateRetVoid();
            } else {
                //return default value based on return type
                llvm::Value* defaultValue = llvm::UndefValue::get(_returnType->GetType(context));
                context.builder().CreateRet(defaultValue);
            }
        }
        return func;
    }
    

    //Variable Declaration
    llvm::Value* VarDec::CodeGen(CodeGenContext& context){
        if(_VarList != NULL){
            for(auto& var : *(_VarList)){
                var->CodeGen(context);  //Generate code for each var
            }
        }
        return NULL;
    }

    //Variable Initialization
    llvm::Value* VarInit::CodeGen(CodeGenContext& context){
        llvm::Value* initVal = NULL;
        if(_value != NULL){
            initVal = _value->CodeGen(context); //Generate code for init value
        }
        llvm::AllocaInst* alloca = context.createEntryBlockAlloca(_VarType->GetType(context), _name);
        context.createSymbolTableEntry(_name, alloca);
        if(initVal != NULL){
            context.builder().CreateStore(initVal, alloca);
        }
        return NULL;
    }

    //Type Declaration
    llvm::Value* TypeDec::CodeGen(CodeGenContext& context){
        llvm::Type* type = this->_type->GetType(context);
        context.createSymbolTableEntry(_alias, type);
        return NULL;
    }

    //Basic Type
    llvm::Type* BasicType::GetType(CodeGenContext& context){
        llvm::LLVMContext& llvmContext = context.getLLVMContext();
        switch (this->_type) {
            case _int:
                return llvm::Type::getInt32Ty(llvmContext);
            case _float:
                return llvm::Type::getFloatTy(llvmContext);
            case _char:
                return llvm::Type::getInt8Ty(llvmContext);
            default:
                //Unkown type
                std::cerr << "Unkown type" << std::endl;
                return NULL;
        }
    }

    //Array Type
    llvm::Type* ArrayType::GetType(CodeGenContext& context){
        llvm::Type* elementType = this->_type->GetType(context);
        if (!elementType) {
            //Invalid element type
            std::cerr << "Invalid element type" << std::endl;
            return NULL;
        }
        //create array type
        llvm::Type* arrayType = llvm::ArrayType::get(elementType, this->_size);
        return arrayType;
    }

    //Struct Type
    llvm::Type* StructType::GetType(CodeGenContext& context){
        //if the struct is already defined, return the type
        if (_type) {
            return _type;
        }
        //else, create a new struct type
        _type = GenerateStruct(context);
        if(!_type){
            //Invalid struct type
            return NULL;
        }
        //generate code for struct body
        if (!GenerateBody(context)) {
            //Invalid struct body
            std::cerr << "Invalid struct body" << std::endl;
            return NULL;
        }
        return _type;
    }

    llvm::Type* StructType::GenerateStruct(CodeGenContext& context, std::string name){
        llvm::LLVMContext& llvmContext = context.getLLVMContext();
        return llvm::StructType::create(llvmContext, name);
    }

    llvm::Type* StructType::GenerateBody(CodeGenContext& context){
        llvm::StructType* structType = llvm::cast<llvm::StructType>(_type);
        if (!structType) {
            //Invalid struct type
            std::cerr << "Invalid struct type" << std::endl;
            return NULL;
        }
        if (!_structbody){
            //Empty struct body
            std::cerr << "Empty struct body" << std::endl;
            return structType;
        }
        std::vector<llvm::Type*> fieldTypes;
        for(auto field : *(this->_structbody)){
            llvm::Type* fieldType = field->_type->GetType(context);
            if (!fieldType) {
                //Invalid field type
                std::cerr << "Invalid field type" << std::endl;
                return NULL;
            }
            fieldTypes.push_back(fieldType);
        }
        structType->setBody(fieldTypes);
        return structType;
    }

    //CompStmt
    llvm::Value* CompStmt::CodeGen(CodeGenContext& context){
        //create a new symbol table for this scope
        context.pushSymbolTable();
        //generate local variable declarations
        if(_varDecs != NULL){
            for(VarDec* varDec : *_varDecs){
                varDec->CodeGen(context);
            }
        }
        //generate statements
        llvm::Value* lastValue = NULL;
        if(_stmts != NULL){
            for(Statement* stmt : *_stmts){
                lastValue = stmt->CodeGen(context);
                if (!lastValue) {
                    //Invalid statement
                    std::cerr << "Invalid statement" << std::endl;
                    return NULL;
                }
            }
        }
        //pop the symbol table
        context.popSymbolTable();
        return lastValue;
    }


    //If Statement
    llvm::Value* IfStmt::CodeGen(CodeGenContext& context){
        //generate code for condition
        llvm::Value* condValue = this->_cond->CodeGen(context);
        if (!condValue) {
            //Invalid condition
            return NULL;
        }
        //convert condition to bool
        condValue = context.builder().CreateFCmpONE(condValue, llvm::ConstantInt::get(context.getLLVMContext(), llvm::APInt(32, 0)), "ifcond");
        //get current function
        llvm::Function* func = context.builder().GetInsertBlock()->getParent();
        //create blocks for then and else
        llvm::BasicBlock* thenBlock = llvm::BasicBlock::Create(context.getLLVMContext(), "then", func);
        llvm::BasicBlock* elseBlock = llvm::BasicBlock::Create(context.getLLVMContext(), "else");
        llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(context.getLLVMContext(), "ifcont");
        //create conditional branch
        context.builder().CreateCondBr(condValue, thenBlock, elseBlock);
        //generate code for then
        context.builder().SetInsertPoint(thenBlock);
        llvm::Value* thenValue = _then->CodeGen(context);
        if (!thenValue) {
            //Invalid then
            std::cerr << "Invalid then" << std::endl;
            return NULL;
        }
        context.builder().CreateBr(mergeBlock);
        thenBlock = context.builder().GetInsertBlock(); //update then block for phi node
        //generate code for else if it exists
        func->getBasicBlockList().push_back(elseBlock);
        context.builder().SetInsertPoint(elseBlock);
        llvm::Value* elseValue = NULL;
        if (_else) {
            elseValue = _else->CodeGen(context);
            if (!elseValue) {
                //Invalid else
                std::cerr << "Invalid else" << std::endl;
                return NULL;
            }
        }
        context.builder().CreateBr(mergeBlock);
        elseBlock = context.builder().GetInsertBlock(); //update else block for phi node
        //generate code for merge
        func->getBasicBlockList().push_back(mergeBlock);
        context.builder().SetInsertPoint(mergeBlock);
        llvm::PHINode* phiNode = context.builder().CreatePHI(llvm::Type::getVoidTy(context.getLLVMContext()), 2, "iftmp");
        phiNode->addIncoming(thenValue, thenBlock);
        if (_else) {
            phiNode->addIncoming(elseValue, elseBlock);
        }
        return phiNode;
    }

    //Return Statement
    llvm::Value* RetStmt::CodeGen(CodeGenContext& context){
        if (_retval) {
            //generate code for return value
            llvm::Value* retVal = this->_retval->CodeGen(context);
            if (!retVal) {
                //Invalid return value
                std::cerr << "Invalid return value" << std::endl;
                return NULL;
            }
            //create return instruction
            context.builder().CreateRet(retVal);
        } else {
            //create return void instruction
            context.builder().CreateRetVoid();
        }
        return NULL;    //return void
    }

    //While Statement
    llvm::Value* WhileStmt::CodeGen(CodeGenContext& context){
        //create blocks for condition, body and merge
        llvm::Function* func = context.builder().GetInsertBlock()->getParent();
        llvm::BasicBlock* condBlock = llvm::BasicBlock::Create(context.getLLVMContext(), "whilecond", func);
        llvm::BasicBlock* bodyBlock = llvm::BasicBlock::Create(context.getLLVMContext(), "whilebody");
        llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(context.getLLVMContext(), "whilecont");
        //jump to condition block
        context.builder().CreateBr(condBlock);
        //generate code for loop condition
        context.builder().SetInsertPoint(condBlock);
        llvm::Value* condValue = _cond->CodeGen(context);
        if (!condValue) {
            //Invalid condition
            std::cerr << "Invalid condition" << std::endl;
            return NULL;
        }
        //jump to body when condition is true
        context.builder().CreateCondBr(condValue, bodyBlock, mergeBlock);
        this->_body->CodeGen(context);
        //jump back to condition block
        context.builder().CreateBr(condBlock);
        //set insert point to merge block
        context.builder().SetInsertPoint(mergeBlock);
        return NULL;    //return void
    }

    //For Statement
    llvm::Value* ForStmt::CodeGen(CodeGenContext& context){
        //create blocks for init, condition, body and merge
        llvm::Function* func = context.builder().GetInsertBlock()->getParent();
        llvm::BasicBlock* initBlock = llvm::BasicBlock::Create(context.getLLVMContext(), "forinit", func);
        llvm::BasicBlock* condBlock = llvm::BasicBlock::Create(context.getLLVMContext(), "forcond");
        llvm::BasicBlock* bodyBlock = llvm::BasicBlock::Create(context.getLLVMContext(), "forbody");
        llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(context.getLLVMContext(), "forcont");
        //jump to init block before loop
        context.builder().CreateBr(initBlock);
        //generate code for init
        context.builder().SetInsertPoint(initBlock);
        if (this->_init) {
            this->_init->CodeGen(context);
        }
        //jump to condition block
        context.builder().CreateBr(condBlock);
        //generate code for loop condition
        context.builder().SetInsertPoint(condBlock);
        llvm::Value* condValue = NULL;
        if (this->_cond) {
            condValue = this->_cond->CodeGen(context);
            if (!condValue) {
                //Invalid condition
                std::cerr << "Invalid condition" << std::endl;
                return NULL;
            }
        } else {
            //if no condition, set condition to true
            condValue = llvm::ConstantInt::get(context.getLLVMContext(), llvm::APInt(32, 1));
        }
        //jump to body when condition is true
        context.builder().CreateCondBr(condValue, bodyBlock, mergeBlock);
        //generate code for loop body
        context.builder().SetInsertPoint(bodyBlock);
        if (this->_body) {
            this->_body->CodeGen(context);
        }
        //generate code for loop increment
        if (this->_step) {
            this->_step->CodeGen(context);
        }
        //jump back to condition block
        context.builder().CreateBr(condBlock);
        //set insert point to merge block
        context.builder().SetInsertPoint(mergeBlock);
        return NULL;    //return void
    }

    //Exp ASSIGNOP Exp
    llvm::Value* AssignOpExpr::CodeGen(CodeGenContext& context){
        //generate code for left and right-hand side
        llvm::Value* lval = LHS->CodeGen(context);
        llvm::Value* rval = RHS->CodeGen(context);
        //get type of left-hand side
        llvm::Type* lType = lval->getType();
        //convert right-hand side to left-hand side type
        rval = context.builder().CreateIntCast(rval, lType, true);
        //store right-hand side to into left-hand side
        context.builder().CreateStore(rval, lval);
        return rval;
    }

    //Exp BinOP RHS
    llvm::Value* BinaryOpExpr::CodeGen(CodeGenContext& context){
        //generate code for left and right-hand side
        llvm::Value* lval = this->LHS->CodeGen(context);
        llvm::Value* rval = this->RHS->CodeGen(context);
        //perform type checking
        llvm::Type* ltype = lval->getType();
        llvm::Type* rtype = rval->getType();
        if (ltype != rtype) {
            //if type mismatch, convert right-hand side to left-hand side type
            if (ltype ->isIntegerTy() && rtype->isIntegerTy()) {
                rval = context.builder().CreateIntCast(rval, ltype, true);
            } else if (ltype->isFloatingPointTy() && rtype->isFloatingPointTy()) {
                rval = context.builder().CreateFPCast(rval, ltype);
            } else {
                //Invalid type
                std::cerr << "Invalid type" << std::endl;
                return NULL;
            }
        }
        llvm::Value* ret = NULL;
        //perform binary operation
        if (Operator == "+") {
            ret = context.builder().CreateAdd(lval, rval, "addtmp");
        } else if (Operator == "-") {
            ret =  context.builder().CreateSub(lval, rval, "subtmp");
        } else if (Operator == "*") {
            return context.builder().CreateMul(lval, rval, "multmp");
        } else if (Operator == "/") {
            return context.builder().CreateSDiv(lval, rval, "divtmp");
        } else if (Operator == "%") {
            return context.builder().CreateSRem(lval, rval, "modtmp");
        } else if (Operator == "<") {
            return context.builder().CreateICmpSLT(lval, rval, "lttmp");
        } else if (Operator == ">") {
            return context.builder().CreateICmpSGT(lval, rval, "gttmp");
        } else if (Operator == "<=") {
            return context.builder().CreateICmpSLE(lval, rval, "letmp");
        } else if (Operator == ">=") {
            return context.builder().CreateICmpSGE(lval, rval, "getmp");
        } else if (Operator == "==") {
            return context.builder().CreateICmpEQ(lval, rval, "eqtmp");
        } else if (Operator == "!=") {
            return context.builder().CreateICmpNE(lval, rval, "netmp");
        } else if (Operator == "&&") {
            return context.builder().CreateAnd(lval, rval, "andtmp");
        } else if (Operator == "||") {
            return context.builder().CreateOr(lval, rval, "ortmp");
        } else {
            //Invalid operator
            std::cerr << "Invalid operator" << std::endl;
            return NULL;
        }
        return ret;
    }

    //Minus Expression
    llvm::Value* MinusExpr::CodeGen(CodeGenContext& context){
        //generate code for expression
        llvm::Value* val = this->Expr->CodeGen(context);
        //generate code for minus operation
        return context.builder().CreateNeg(val, "negtmp");
    }

    //Not Expression
    llvm::Value* NotExpr::CodeGen(CodeGenContext& context){
        //generate code for expression
        llvm::Value* val = Expr->CodeGen(context);
        //generate code for not operation
        return context.builder().CreateNot(val, "nottmp");
    }

    //Exp LB Exp RB
    llvm::Value* ArrayVisitExpr::CodeGen(CodeGenContext& context){
        //generate code for array and index
        llvm::Value* array = Array->CodeGen(context);
        llvm::Value* index = Index->CodeGen(context);
        //convert index to integer
        index = context.builder().CreateIntCast(index, llvm::Type::getInt32Ty(context.getLLVMContext()), true);
        //calculate address of element
        llvm::Value* addr = context.builder().CreateGEP(array, index, "arraytmp");
        //load element
        return context.builder().CreateLoad(addr, "arrayloadtmp");
    }

    //INT
    llvm::Value* IntExpr::CodeGen(CodeGenContext& context){
        //create LLVM integer type
        llvm::Type* intType = llvm::Type::getInt32Ty(context.getLLVMContext());
        //create integer constant
        llvm::Value* intConst = llvm::ConstantInt::get(intType, this->value, true);
        return intConst;        
    }

    //FLOAT
    llvm::Value* FloatExpr::CodeGen(CodeGenContext& context){
        //create LLVM float type
        llvm::Type* floatType = llvm::Type::getFloatTy(context.getLLVMContext());
        //create float constant
        double val = static_cast<double>(this->value);
        llvm::Value* floatConst = llvm::ConstantFP::get(floatType, val);
        return floatConst;
    }

    //ID
    llvm::Value* IDExpr::CodeGen(CodeGenContext& context){
        //search variable from symbol table
        llvm::Value* variable = context.getSymbolTableEntry(name);
        if (!variable) {
            std::cerr << "Error: Variable " << name << " not declared" << std::endl;
            return NULL;
        }
        return variable;
    }

    //Exp DOT ID
    llvm::Value* StructVisitExpr::CodeGen(CodeGenContext& context){
        //generate code for struct pointer
        llvm::Value* structPtr = Struct->CodeGen(context);
        //check if struct pointer is null
        if (!structPtr) {
            std::cerr << "Error: Struct pointer is null" << std::endl;
            return NULL;
        }
        //change struct pointer to correct type
        llvm::PointerType* structPtrType = llvm::dyn_cast<llvm::PointerType>(structPtr->getType());
        llvm::StructType* structType = llvm::dyn_cast<llvm::StructType>(structPtrType->getElementType());
        //get index of struct element
        unsigned int index = context.getStructMemberIndex(structType, name);
        if (index == -1) {
            std::cerr << "Error: Struct member " << name << " not found" << std::endl;
            return NULL;
        }
        //visit struct element with index
        llvm::Value* structElement = context.builder().CreateStructGEP(structType, structPtr, index, "structtmp");
        return structElement;
    }

    //ID LP Args RP, ID LP RP
    llvm::Value* CallFuncExpr::CodeGen(CodeGenContext& context){
        //get function from symbol table
        llvm::Function* function = context.getFunction(_name);
        if (!function) {
            std::cerr << "Error: Function " << _name << " not declared" << std::endl;
            return NULL;
        }
        //generate arguments list
        std::vector<llvm::Value*> args;
        if (_args) {
            for (Expression* arg : *_args) {
                args.push_back(arg->CodeGen(context));
            }
        }
        //generate call instruction
        return context.builder().CreateCall(function, args, "calltmp");
    }
}