#include"CodeGenContext.hpp"
#include"AST.hpp"

namespace AST{
    //Program 
    llvm::Value* Program::CodeGen(CodeGenContext& context){
        for(auto& dec : *(this->_decs)){
            dec->CodeGen(context);
        }
        return NULL;
    }

    //Function Declaration
    llvm::Value* FunDec::CodeGen(CodeGenContext& context){
        std::vector<llvm::Type*> argTypes;
        //argument type list
        if(_args != NULL){
            for(auto arg : *(this->_args)){
                llvm::Type* argType = arg->_type->GetType(context);
                //handle array type
                if(ArrayType* arrayType = dynamic_cast<ArrayType*>(arg->_type)){
                    if (arrayType->_size == 0) {
                        //if empty array, pass a pointer
                        llvm::Type* elementType = arrayType->_type->GetType(context);
                        llvm::PointerType* pointerType = llvm::PointerType::get(elementType, 0);
                        argTypes.push_back(pointerType);
                    } else {
                        //if not empty array, pass the array type
                        argTypes.push_back(argType);
                    }
                } else {
                    //if not array, pass the type
                    llvm::Type* argType = arg->_type->GetType(context);
                    argTypes.push_back(argType);
                }
            }
        }
        //get function type
        llvm::FunctionType* funcType = llvm::FunctionType::get(this->_returnType->GetType(context), argTypes, _is_va);
        //create function
        llvm::Function* func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, this->_name, context.getModule());
        context.AddFunc(this->_name, func);
        if(_body!=NULL){
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
        }
        return func;
    }
    

    //Variable Declaration
    llvm::Value* VarDec::CodeGen(CodeGenContext& context){
        if(_VarList != NULL){
            for(auto& var : *(_VarList)){
                var->_VarType = this->_VarType;
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
            if(initVal->getType() != _VarType->GetType(context)){
                initVal = context.builder().CreateIntCast(initVal, _VarType->GetType(context) , true);
            }
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
        if (this->_size == 0) {
            //create pointer type
            return llvm::PointerType::get(elementType, 0);
        }
        //create array type
        return llvm::ArrayType::get(elementType, this->_size);
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
                // if (!lastValue) {
                //     //Invalid statement
                //     std::cerr << "Invalid statement" << std::endl;
                //     return NULL;
                // }
            }
        }
        //pop the symbol table
        context.popSymbolTable();
        return lastValue;
    }


    llvm::Value* ExpStmt::CodeGen(CodeGenContext& context){
        _exp->CodeGen(context);
        return NULL;
    }

    //If Statement
    llvm::Value* IfStmt::CodeGen(CodeGenContext& context){
        //get current function
        llvm::Function* func = context.builder().GetInsertBlock()->getParent();
        //create blocks for then and else
        llvm::BasicBlock* thenBlock = llvm::BasicBlock::Create(context.getLLVMContext(), "then", func);
        llvm::BasicBlock* elseBlock = llvm::BasicBlock::Create(context.getLLVMContext(), "else",func);
        llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(context.getLLVMContext(), "ifcont",func);
        //generate code for condition
        llvm::Value* condValue = this->_cond->CodeGen(context);
        if (!condValue) {
            //Invalid condition
            std::cerr << "Invalid condition" << std::endl;
            return NULL;
        }
        //create conditional branch
        context.builder().CreateCondBr(condValue, thenBlock, elseBlock);
        //generate code for then
        // func->getBasicBlockList().push_back(thenBlock);
        context.builder().SetInsertPoint(thenBlock);
        llvm::Value* thenValue = _then->CodeGen(context);
        context.builder().CreateBr(mergeBlock);
        context.builder().SetInsertPoint(elseBlock);
        llvm::Value* elseValue = NULL;
        context.builder().CreateBr(mergeBlock);
        //generate code for merge
        context.builder().SetInsertPoint(mergeBlock);
        return NULL;
    }

    //Return Statement
    llvm::Value* RetStmt::CodeGen(CodeGenContext& context){
        bool contextSave = context._is_save;
        
        if (_retval) {
            //generate code for return value
            context._is_save = false;
            llvm::Value* retVal = this->_retval->CodeGen(context);
            context._is_save = true;
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
        llvm::Function* func = context.builder().GetInsertBlock()->getParent();
        llvm::BasicBlock* condBlock = llvm::BasicBlock::Create(context.getLLVMContext());
        context.builder().SetInsertPoint(condBlock);
        return NULL;    //return void
    }

    //While Statement
    llvm::Value* WhileStmt::CodeGen(CodeGenContext& context){
        //create blocks for condition, body and merge
        llvm::Function* func = context.builder().GetInsertBlock()->getParent();
        llvm::BasicBlock* condBlock = llvm::BasicBlock::Create(context.getLLVMContext(), "whilecond", func);
        llvm::BasicBlock* bodyBlock = llvm::BasicBlock::Create(context.getLLVMContext(), "whilebody",func);
        llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(context.getLLVMContext(), "whilecont",func);
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
        context.builder().SetInsertPoint(bodyBlock);
        this->_body->CodeGen(context);
        //jump back to condition block
        context.builder().CreateBr(condBlock);
        //set insert point to merge block
        context.builder().SetInsertPoint(mergeBlock);
        return NULL;    //return void
    }

    //Exp ASSIGNOP Exp
    llvm::Value* AssignOpExpr::CodeGen(CodeGenContext& context){
        bool contextSave = context._is_save;
        context._is_save = true;
        //generate code for left and right-hand side
        context._is_save = true;
        llvm::Value* lval = LHS->CodeGen(context);
        context._is_save = false;
        llvm::Value* rval = RHS->CodeGen(context);
        //type cast
        llvm::Type* ltype = lval->getType();
        if(lval->getType()->getTypeID() == llvm::Type::PointerTyID) {
            ltype = ltype->getPointerElementType();
        } else if (lval->getType()->getTypeID() == llvm::Type::ArrayTyID) {
            ltype = ltype->getArrayElementType();
        }
        llvm::Type* rtype = rval->getType();
        if (rtype != ltype) {
            rval = context.builder().CreateIntCast(rval, ltype, true);
        }
        //store right-hand side to into left-hand side
        context.builder().CreateStore(rval, lval);
        context._is_save = contextSave;
        return rval;
    }

    //Exp BinOP RHS
    llvm::Value* BinaryOpExpr::CodeGen(CodeGenContext& context){
        bool contextSave = context._is_save;
        context._is_save = false;
        //generate code for left and right-hand side
        llvm::Value* lval = this->LHS->CodeGen(context);
        llvm::Value* rval = this->RHS->CodeGen(context);
        //type cast
        llvm::Type* ltype = lval->getType();
        if(lval->getType()->getTypeID() == llvm::Type::PointerTyID) {
            ltype = ltype->getPointerElementType();
        } else if (lval->getType()->getTypeID() == llvm::Type::ArrayTyID) {
            ltype = ltype->getArrayElementType();
        }
        llvm::Type* rtype = rval->getType();
        if (rtype != ltype) {
            if (ltype->isIntegerTy() && rtype->isFloatTy()) {
                lval = context.builder().CreateSIToFP(lval, rtype);
            } else if (ltype->isFloatTy() && rtype->isIntegerTy()) {
                rval = context.builder().CreateSIToFP(rval, ltype);
            }
        }
        llvm::Value* ret = NULL;
        //perform binary operation
        if (Operator == "+") {
            if ( lval->getType()->isFloatTy() )
                ret = context.builder().CreateFAdd(lval, rval, "addtmp");
            else if ( lval->getType()->isIntegerTy() )
                ret = context.builder().CreateAdd(lval, rval, "addtmp");
        } else if (Operator == "-") {
            if ( lval->getType()->isFloatTy() )
                ret = context.builder().CreateFSub(lval, rval, "subtmp");
            else if ( lval->getType()->isIntegerTy() )
                ret = context.builder().CreateSub(lval, rval, "subtmp");
        } else if (Operator == "*") {
            if ( lval->getType()->isFloatTy() )
                ret = context.builder().CreateFMul(lval, rval, "multmp");
            else if ( lval->getType()->isIntegerTy() )
                ret = context.builder().CreateMul(lval, rval, "multmp");
        } else if (Operator == "/") {
            if ( lval->getType()->isFloatTy() )
                ret = context.builder().CreateFDiv(lval, rval, "divtmp");
            else if ( lval->getType()->isIntegerTy() )
                ret = context.builder().CreateSDiv(lval, rval, "divtmp");
        } else if (Operator == "%") {
            if ( lval->getType()->isFloatTy() )
                ret = context.builder().CreateFRem(lval, rval, "modtmp");
            else if ( lval->getType()->isIntegerTy() )
                ret = context.builder().CreateSRem(lval, rval, "modtmp");
        } else if (Operator == "<") {
            if ( lval->getType()->isFloatTy() )
                ret =  context.builder().CreateFCmpOLT(lval, rval, "lttmp");
            else if ( lval->getType()->isIntegerTy() )
                ret = context.builder().CreateICmpSLT(lval, rval, "lttmp");
        } else if (Operator == ">") {
            if ( lval->getType()->isFloatTy() )
                ret =  context.builder().CreateFCmpOGT(lval, rval, "gttmp");
            else if ( lval->getType()->isIntegerTy() )
                ret = context.builder().CreateICmpSGT(lval, rval, "gttmp");
        } else if (Operator == "<=") {
            if ( lval->getType()->isFloatTy() )
                ret =  context.builder().CreateFCmpOLE(lval, rval, "letmp");
            else if ( lval->getType()->isIntegerTy() )
                ret = context.builder().CreateICmpSLE(lval, rval, "letmp");
        } else if (Operator == ">=") {
            if ( lval->getType()->isFloatTy() )
                ret =  context.builder().CreateFCmpOGE(lval, rval, "getmp");
            else if ( lval->getType()->isIntegerTy() )
                ret = context.builder().CreateICmpSGE(lval, rval, "getmp");
        } else if (Operator == "==") {
            if ( lval->getType()->isFloatTy() )
                ret =  context.builder().CreateFCmpOEQ(lval, rval, "eqtmp");
            else if ( lval->getType()->isIntegerTy() )
                ret = context.builder().CreateICmpEQ(lval, rval, "eqtmp");
        } else if (Operator == "!=") {
            if ( lval->getType()->isFloatTy() )
                ret =  context.builder().CreateFCmpONE(lval, rval, "netmp");
            else if ( lval->getType()->isIntegerTy() )
                ret = context.builder().CreateICmpNE(lval, rval, "netmp");
        } else if (Operator == "&&") {
            ret = context.builder().CreateAnd(lval, rval, "andtmp");
        } else if (Operator == "||") {
            ret = context.builder().CreateOr(lval, rval, "ortmp");
        } else {
            //Invalid operator
            std::cerr << "Invalid operator" << std::endl;
        }
        context._is_save = contextSave;
        return ret;
    }

    //Minus Expression
    llvm::Value* MinusExpr::CodeGen(CodeGenContext& context){
        bool contextSave = context._is_save;
        context._is_save = false;
        //generate code for expression
        llvm::Value* val = this->Expr->CodeGen(context);
        //generate code for minus operation
        context._is_save = contextSave;
        return context.builder().CreateNeg(val, "negtmp");
    }

    //Not Expression
    llvm::Value* NotExpr::CodeGen(CodeGenContext& context){
        bool contextSave = context._is_save;
        context._is_save = false;
        //generate code for expression
        llvm::Value* val = Expr->CodeGen(context);
        //generate code for not operation
        context._is_save = contextSave;
        return context.builder().CreateNot(val, "nottmp");
    }

    //Exp LB Exp RB
llvm::Value* ArrayVisitExpr::CodeGen(CodeGenContext& context){ 
        //generate code for array and index
        bool contextSave = context._is_save;
        context._is_save = true;
        llvm::Value* array = Array->CodeGen(context);
        context._is_save = false;
        llvm::Value* index = Index->CodeGen(context);

        //convert index to integer
        auto zero = context.builder().getInt32(0);
        index = context.builder().CreateIntCast(index, llvm::Type::getInt32Ty(context.getLLVMContext()), true);
        //calculate address of element
        llvm::Value* addr;
        if(array->getType()->getPointerElementType()->isArrayTy()){
            addr = context.builder().CreateGEP(array, {zero,index}, "arraytmp");
        }else{
            array = context.builder().CreateLoad(array, "arraytmp");
            addr = context.builder().CreateGEP(array ,index, "arraytmp");
        }
        // addr = context.builder().CreateGEP(array, {zero,index}, "arraytmp");
        // llvm::Value* addr = context.builder().CreateExtractValue(index,array);
        //load element
        context._is_save = contextSave;
        llvm::Value* ret;
        if(contextSave){
            ret = addr;
        }else{
            ret = context.builder().CreateLoad(addr, "arrayloadtmp");
        }
        return ret;
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
        if(!context._is_save){
            llvm::Value * load = context.builder().CreateLoad(variable);
            return load;
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
        bool contextSave = context._is_save;
        //get function from symbol table
        llvm::Function* function = context.getFunction(_name);
        if (!function) {
            std::cerr << "Error: Function " << _name << " not declared" << std::endl;
            return NULL;
        }
        //generate arguments list
        std::vector<llvm::Value*> args;
        if (_args) {
            for (auto arg = _args->rbegin();arg!=_args->rend();arg++) {
                auto zero = context.builder().getInt32(0);
                //calculate address of element
                llvm::Value* addr;
                context._is_save = true;
                llvm::Value* argument = (*arg)->CodeGen(context);
                context._is_save = contextSave;
                if(argument->getType()->isPointerTy()){
                    if(argument->getType()->getPointerElementType()->isArrayTy()){
                        argument = context.builder().CreateGEP(argument, {zero,zero}, "arraytmp");
                    }else{
                        argument = context.builder().CreateLoad(argument);
                    }
                }
                args.push_back(argument);
            }
        }
        //generate call instruction
        return context.builder().CreateCall(function, args, "calltmp");
    }
}