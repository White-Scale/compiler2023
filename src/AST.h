#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <stack>
#include <string>
#include <exception>
#include <llvm/IR/Value.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/IR/ValueSymbolTable.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Verifier.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/Interpreter.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/DynamicLibrary.h>
#include <llvm/Target/TargetMachine.h>

namespace AST{
    class Node;

    class Program : public Node;

    class Declaration : public Node;
        class VarDec : public Declaration;
        class FunDec : public Declaration;

    class Statement : public Node;
        class CompStmt : public Statement;  //LC DefList StmtList RC, Statement Blocks
        class IfStmt : public Statement;    //IF LP Exp RP Stmt
        class RetStmt : public Statement;   //RETURN Exp SEMI
        class IfElseStmt : public Statement;    //IF LP Exp RP Stmt ELSE Stmt
        class WhileStmt : public Statement; //WHILE LP Exp RP Stmt

    class Definition : public Node;     //Def DefList

    class Expression : public Node;
        class AssignOpExpr : public Expression; //Exp ASSIGNOP Exp
        class BinaryOpExpr : public Expression; //Exp BINOP Exp
        class ParenExpr : public Expression;    //LP Exp RP
        class MinusExpr : public Expression;    //MINUS Exp
        class NotExpr : public Expression;      //NOT Exp
        class CallFuncExpr : public Expression; //ID LP Args RP, ID LP RP
        class ArrayVisitExpr : public Expression;   //Exp LB Exp RB
        class StructVisitExpr : public Expression;  //Exp DOT ID
        class IntExpr : public Expression;      //INT
        class FloatExpr : public Expression;    //FLOAT
        class IDExpr : public Expression;       //ID
}
//TODO: 是否要做结构体（定义及其访问）
//TODO: Specifier等类似乎不太好抽象出节点来，不知道需要么