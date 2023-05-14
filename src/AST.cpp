//暂时忽略所有错误检查和类型检查/转换
//缺少注释
//Todo: BinaryOpExpr, ArrayVisitExpr, IDExpr, CompStmt(?)

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
        for(auto arg : *(this->_args)){
            argTypes.push_back(arg->_type->GetType(context));
        }
        //get function type
        llvm::FunctionType* funcType = llvm::FunctionType::get(this->_returnType->GetType(context), argTypes, false);
        llvm::Function* func = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, this->_name, context.module);
        context.AddFunc(this->_name, func);
        context.EnterFunc(func);
        for(auto arg : *(this->_args)){
            arg->CodeGen(context);
        }
        this->_body->CodeGen(context);
        context.ExitFunc();
        return func;
    }
    
    //Function body
    llvm::Value* FunBody::CodeGen(CodeGenContext& context){
        for(auto& stmt : *(this->_stmts)){
            stmt->CodeGen(context);
        }
        return NULL;
    }

    //Variable Declaration
    llvm::Value* VarDec::CodeGen(CodeGenContext& context){
        llvm::Type* type = this->_VarType->GetType(context);
        for(auto& var : *(this->_VarList)) {
            context.AddVar(var->_name, var->_value->CodeGen(context));
        }
        return NULL;
    }

    //Type Declaration
    llvm::Value* TypeDec::CodeGen(CodeGenContext& context){
        llvm::Type* type;
        if(this->_type->isStructType()) {
            type = ((AST::StructType*)this->_type)->GenerateStruct(context, this->_alias);
            ((AST::StructType*)this->_type)->GenerateBody(context);
        } else {
            type = this->_type->GetType(context);
        }
        return NULL;
    }

    //Basic Type
    llvm::Type* BasicType::GetType(CodeGenContext& context){
        switch(this->_type){
            case _int:
                return llvm::Type::getInt32Ty(context.module->getContext());
            case _float:
                return llvm::Type::getFloatTy(context.module->getContext());
            case _char:
                return llvm::Type::getInt8Ty(context.module->getContext());
            case _void:
                return llvm::Type::getVoidTy(context.module->getContext());
            case _bool:
                return llvm::Type::getInt1Ty(context.module->getContext());
            default:
                return NULL;
        }
    }

    //Array Type
    llvm::Type* ArrayType::GetType(CodeGenContext& context){
        //only one dimension
        llvm::Type* type = this->_type->GetType(context);
        return llvm::ArrayType::get(type, this->_size);
    }

    //Struct Type
    llvm::Type* StructType::GetType(CodeGenContext& context){
        this->GenerateStruct(context, "");
        return this->GenerateBody(context);
    }
    llvm::Type* StructType::GenerateStruct(CodeGenContext& context, std::string name){
        //struct type
        auto type = llvm::StructType::create(context.module->getContext(), "struct."+name);
        return this->_type = type;
    }
    llvm::Type* StructType::GenerateBody(CodeGenContext& context){
        //each field type
        std::vector<llvm::Type*> fieldTypes;
        for(auto field : *(this->_structbody)){
            fieldTypes.push_back(field->_type->GetType(context));
        }
        ((llvm::StructType*)this->_type)->setBody(fieldTypes);
        return this->_type;
    }

    //If Statement
    llvm::Value* IfStmt::CodeGen(CodeGenContext& context){
        //if(cond) {then} else {else} merge
        llvm::Value* cond = this->_cond->CodeGen(context);
        llvm::Function* func = context.GetCurrFunc();
        llvm::BasicBlock* thenBlock = llvm::BasicBlock::Create(context.module->getContext(), "then", func);
        llvm::BasicBlock* elseBlock = llvm::BasicBlock::Create(context.module->getContext(), "else");
        llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(context.module->getContext(), "ifcont");
        context.SetInsertPoint(thenBlock);
        this->_then->CodeGen(context);
        context.SetInsertPoint(elseBlock);
        this->_else->CodeGen(context);
        context.SetInsertPoint(mergeBlock);
        return NULL;
    }

    //Return Statement
    llvm::Value* RetStmt::CodeGen(CodeGenContext& context){
        llvm::Value* retVal = this->_retval->CodeGen(context);
        llvm::Function* func = context.GetCurrFunc();
        llvm::BasicBlock* currBlock = context.GetInsertPoint();
        llvm::BasicBlock* retBlock = llvm::BasicBlock::Create(context.module->getContext(), "return", func);
        context.SetInsertPoint(retBlock);
        llvm::ReturnInst::Create(context.module->getContext(), retVal, retBlock);
        context.SetInsertPoint(currBlock);
        return NULL;
    }

    //While Statement
    llvm::Value* WhileStmt::CodeGen(CodeGenContext& context){
        //while(cond) {body} merge
        llvm::Function* func = context.GetCurrFunc();
        llvm::BasicBlock* condBlock = llvm::BasicBlock::Create(context.module->getContext(), "cond", func);
        llvm::BasicBlock* bodyBlock = llvm::BasicBlock::Create(context.module->getContext(), "body");
        llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(context.module->getContext(), "whilecont");
        context.SetInsertPoint(condBlock);
        llvm::Value* cond = this->_cond->CodeGen(context);
        llvm::BranchInst::Create(bodyBlock, mergeBlock, cond, condBlock);
        context.SetInsertPoint(bodyBlock);
        this->_body->CodeGen(context);
        llvm::BranchInst::Create(condBlock, bodyBlock);
        context.SetInsertPoint(mergeBlock);
        return NULL;
    }

    //For Statement
    llvm::Value* ForStmt::CodeGen(CodeGenContext& context){
        //for(cond) {body} merge
        llvm::Function* func = context.GetCurrFunc();
        llvm::BasicBlock* condBlock = llvm::BasicBlock::Create(context.module->getContext(), "cond", func);
        llvm::BasicBlock* bodyBlock = llvm::BasicBlock::Create(context.module->getContext(), "body");
        llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(context.module->getContext(), "forcont");
        context.SetInsertPoint(condBlock);
        this->_init->CodeGen(context);
        llvm::Value* cond = this->_cond->CodeGen(context);
        llvm::BranchInst::Create(bodyBlock, mergeBlock, cond, condBlock);
        context.SetInsertPoint(bodyBlock);
        this->_body->CodeGen(context);
        this->_step->CodeGen(context);
        llvm::BranchInst::Create(condBlock, bodyBlock);
        context.SetInsertPoint(mergeBlock);
        return NULL;
    }

    //Exp ASSIGNOP Exp
    llvm::Value* AssignOpExpr::CodeGen(CodeGenContext& context){
        llvm::Value* lval = this->LHS->CodeGen(context);
        llvm::Value* rval = this->RHS->CodeGen(context);
        Builder.CreateStore(rval, lval);    //store rval to lval location
        return rval;
    }

    //Exp BinOP RHS
    llvm::Value* BinaryOpExpr::CodeGen(CodeGenContext& context){
        llvm::Value* lval = this->LHS->CodeGen(context);
        llvm::Value* rval = this->RHS->CodeGen(context);
        //Todo switch

    }

    //Minus Expression
    llvm::Value* MinusExpr::CodeGen(CodeGenContext& context){
        llvm::Value* val = this->Expr->CodeGen(context);
        return Builder.CreateNeg(val, "negtmp");
    }

    //Not Expression
    llvm::Value* NotExpr::CodeGen(CodeGenContext& context){
        llvm::Value* val = this->Expr->CodeGen(context);
        return Builder.CreateNot(val, "nottmp");
    }

    //Exp[Exp]
    llvm::Value* ArrayVisitExpr::CodeGen(CodeGenContext& context){
        llvm::Value* array = this->Array->CodeGen(context);
        llvm::Value* index = this->Index->CodeGen(context);
        return Builder.CreateGEP(array, index, "arraytmp"); //我不懂哪错了
    }

    //INT
    llvm::Value* IntExpr::CodeGen(CodeGenContext& context){
        llvm::Type* intType = llvm::Type::getInt32Ty(context.module->getContext());
        llvm::Constant* intConst = llvm::ConstantInt::get(intType, this->value, true);
        return intConst;
    }

    //FLOAT
    llvm::Value* FloatExpr::CodeGen(CodeGenContext& context){
        llvm::Type* floatType = llvm::Type::getFloatTy(context.module->getContext());
        llvm::Constant* floatConst = llvm::ConstantInt::get(floatType, this->value, true);
        return floatConst;
    }

    //ID
    llvm::Value* IDExpr::CodeGen(CodeGenContext& context) {
        llvm::Value* varValue = context.GetVariable(this->name);
        return Builder.CreateLoad(varValue);    //应该和上面同一个问题
    }
}