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

class CodeGenContext;

namespace AST{
    class Node;

    class Program;

    class Declaration;
        class VarDec;
            class VarInit;
            using VarList = std::vector<VarInit*>;
        class FunDec;
            class Arg;
            class ArgList;
            class FunBody;
        class TypeDec;

    class VarType;
        class BasicType;   //INT, FLOAT
        class ArrayType;   //VarType LB INT RB
        class StructType;  //STRUCT Tag
            class FieldDec; //Specifier VarDecList SEMI
            using FieldList = std::vector<std::string>;

    class Statement;
        // class CompStmt;  //LC DefList StmtList RC, Statement Blocks
        class IfStmt;    //IF LP Exp RP Stmt
        class RetStmt;   //RETURN Exp SEMI
        // class IfElseStmt;    //IF LP Exp RP Stmt ELSE Stmt
        class WhileStmt; //WHILE LP Exp RP Stmt
        class ForStmt;   //FOR LP Exp SEMI Exp SEMI Exp RP Stmt

    // class Definition;     //Def DefList

    class Expression;
        class AssignOpExpr; //Exp ASSIGNOP Exp
        class BinaryOpExpr; //Exp BINOP Exp
        class ParenExpr;    //LP Exp RP
        class MinusExpr;    //MINUS Exp
        class NotExpr;      //NOT Exp
        class CallFuncExpr; //ID LP Args RP, ID LP RP
        class ArrayVisitExpr;   //Exp LB Exp RB
        class StructVisitExpr;  //Exp DOT ID
        class IntExpr;      //INT
        class FloatExpr;    //FLOAT
        class IDExpr;       //ID
}

namespace AST {

    //Node interface for AST nodes
    class Node {
        public:
            Node(){}
            ~Node() {}
            virtual llvm::Value* CodeGen(CodeGenContext& context) = 0;
    };

    //Program Interface
    //Program -> DeclarationList
    class Program : public Node {
        public:
            std::vector<Declaration*>* _decs;
            Program(std::vector<Declaration*>* __decs):_decs(__decs){};
            ~Program(){};
            llvm::Value* CodeGen(CodeGenContext& context);
    };
    
    //Declaration Interface
    class Declaration : public Node {
        public:
            Declaration(){}
            ~Declaration(){}
            virtual llvm::Value* CodeGen(CodeGenContext& context) = 0;
    };

    //Varaible Declaration
    //VarDec -> VarType VarList SEMI
    //VarList -> VarList COMMA VarInit | VarInit
    class VarDec : public Declaration {
        public:
            VarType* _VarType;
            VarList* _VarList;
            VarDec():_VarType(nullptr), _VarList(nullptr) {}
            VarDec(VarType* __VarType, VarList* __VarList):_VarType(__VarType), _VarList(__VarList){};
            ~VarDec(){};
            llvm::Value* CodeGen(CodeGenContext& context);
    };

    //a variable declaration with initialization
    //VarInit -> VarName AssignOP VarValue
    class VarInit : public VarDec {
        public:
            std::string _name;      //variable name
            Expression* _value;      // possible value
            VarInit(const std::string& __name, Expression* __value):_name(__name), _value(__value){}
            ~VarInit(){}
            llvm::Value* CodeGen(CodeGenContext& context) {return nullptr;}
    };

    //Function Declaration
    //FunDec -> VarType FunName LP ArgList RP LC FunBody RC
    //FunDec -> VarType FunName LP RP LC FunBody RC
    //ArgList -> ArgList COMMA Arg | Arg
    //FunBody -> StmtList
    class FunDec : public Declaration {
        public:
            VarType* _returnType;   //return type
            std::string _name;      //function name
            ArgList* _args;         //arguments
            FunBody* _body;         //function body
            FunDec() : _returnType(nullptr), _args(nullptr), _body(nullptr) {}
            FunDec(VarType* __returnType, const std::string& __name, ArgList* __args, FunBody* __body):_returnType(__returnType), _name(__name), _args(__args), _body(__body){};
            ~FunDec(){};
            llvm::Value* CodeGen(CodeGenContext& context);
    };  

    //Function argument
    //Arg -> VarType VarName
    class Arg : public FunDec {
        public:
            VarType* _type;
            std::string _name;
            Arg(VarType* __type, std::string __name):_type(__type), _name(__name){};
            ~Arg(){};
            llvm::Value* CodeGen(CodeGenContext& context) {return nullptr;}
    };

    //Function argument list
    class ArgList : public std::vector<Arg*> {
        public:
            ArgList(){};
            ~ArgList(){};
            llvm::Value* CodeGen(CodeGenContext& context) {return nullptr;}
    };

    //Function body
    class FunBody : public FunDec {
        public:
            std::vector<Statement*>* _stmts;
            FunBody(std::vector<Statement*>* __stmts):_stmts(__stmts){};
            ~FunBody(){};
            llvm::Value* CodeGen(CodeGenContext& context);
    };

    //Type Declaration
    class TypeDec : public Declaration {
        public:
            VarType* _type;
            std::string _alias;    // struct _alias{}
            TypeDec(VarType* __type, const std::string& __alias):_type(__type), _alias(__alias){};
            ~TypeDec(){};
            llvm::Value* CodeGen(CodeGenContext& context);
    };

    //Base class for variable type
    class VarType : public Node {
        public:
            llvm::Type* _type;
            VarType(void):_type(nullptr){}
            ~VarType(){}
            virtual llvm::Type* GetType(CodeGenContext& context) = 0;
            virtual llvm::Value* CodeGen(CodeGenContext& context) {return nullptr;}
            virtual bool isBasicType() = 0;
            virtual bool isArrayType() = 0;
            virtual bool isStructType() = 0;
    };

    //Basic Type
    class BasicType : public VarType {
        public:
            enum TypeID {
                _int,
                _float,
                _char,
                _void,
                _bool
            };
            TypeID _type;
            BasicType(TypeID __type):_type(__type){};
            ~BasicType(){};
            llvm::Type* GetType(CodeGenContext& context);
            bool isBasicType() {return true;}
            bool isArrayType() {return false;}
            bool isStructType() {return false;}
    };

    //Array Type
    class ArrayType : public VarType {
        public:
            VarType* _type;
            int _size;
            ArrayType(VarType* __type, int __size):_type(__type), _size(__size){};
            ArrayType(VarType* __type):_type(__type), _size(0){};     //not initialized
            ~ArrayType(){};
            llvm::Type* GetType(CodeGenContext& context);
            bool isBasicType() {return false;}
            bool isArrayType() {return true;}
            bool isStructType() {return false;}
    };

    //Struct Type
    class StructType : public VarType {
        public:
            std::vector<FieldDec*>* _structbody;      //each field in struct body
            StructType():_structbody(nullptr) {}
            StructType(std::vector<FieldDec*>* __structbody):_structbody(__structbody){};
            ~StructType(){};
            llvm::Type* GetType(CodeGenContext& context);
            llvm::Type* GenerateStruct(CodeGenContext& context, std::string name="<unnamed>");  //generate empty struct
            llvm::Type* GenerateBody(CodeGenContext& context);
            bool isBasicType() {return false;}
            bool isArrayType() {return false;}
            bool isStructType() {return true;}
    };

    //Field declaration in struct
    class FieldDec : public StructType {
        public:
            VarType* _type;
            FieldList* _fieldlist;
            FieldDec(VarType* __type, FieldList* __fieldlist):_type(__type), _fieldlist(__fieldlist){};
            ~FieldDec(){};
            llvm::Value* CodeGen(CodeGenContext& context) {return nullptr;}
    };

    //Statement Interface
    class Statement : public Node {
        public:
            Statement(){}
            ~Statement(){}
            virtual llvm::Value* CodeGen(CodeGenContext& context) = 0;
    };

    //Compound Statement
    class CompStmt : public Statement {
        public:
            Statement* _stmt;
            CompStmt(Statement* __stmt):_stmt(__stmt){};
            ~CompStmt(){};
            llvm::Value* CodeGen(CodeGenContext& context);
    };

    //If Statement
    class IfStmt : public Statement {
        public:
            Expression* _cond;
            Statement* _then;
            Statement* _else;
            IfStmt(Expression* __cond, Statement* __then, Statement* __else = nullptr):_cond(__cond), _then(__then), _else(__else){};
            ~IfStmt(){};
            llvm::Value* CodeGen(CodeGenContext& context);
    };

    //Return Statement
    class RetStmt : public Statement {
        public:
            Expression* _retval;
            RetStmt(Expression* __retval):_retval(__retval){};
            ~RetStmt(){};
            llvm::Value* CodeGen(CodeGenContext& context);
    };

    //While Statement
    class WhileStmt : public Statement {
        public:
            Expression* _cond;
            Statement* _body;
            WhileStmt(Expression* __cond, Statement* __body):_cond(__cond), _body(__body){};
            ~WhileStmt(){};
            llvm::Value* CodeGen(CodeGenContext& context);
    };

    //For Statement
    class ForStmt : public Statement {
        public:
            Expression* _init;
            Expression* _cond;
            Expression* _step;
            Statement* _body;
            ForStmt(Expression* __init, Expression* __cond, Expression* __step, Statement* _body):_init(__init), _cond(__cond), _step(__step), _body(_body){};
            ~ForStmt(){};
            llvm::Value* CodeGen(CodeGenContext& context);
    };

    //Expression Interface
    class Expression : public Node {
        public:
            Expression(){}
            ~Expression(){}
            virtual llvm::Value* CodeGen(CodeGenContext& context) = 0;
    };

    //Exp ASSIGN Exp
    class AssignOpExpr : public Expression {
        public:
            Expression* LHS;
            Expression* RHS;
            AssignOpExpr(Expression* LHS, Expression* RHS):LHS(LHS), RHS(RHS){};
            ~AssignOpExpr(){};
            llvm::Value* CodeGen(CodeGenContext& context);
    };

    //Exp BinOP RHS
    class BinaryOpExpr : public Expression{
        public:
            std::string Operator;//AND\OR\RELOP\PLUS...
            Expression* LHS;
            Expression* RHS;
            BinaryOpExpr(std::string Operator, Expression* LHS, Expression* RHS):Operator(Operator), LHS(LHS), RHS(RHS){};
            ~BinaryOpExpr() {};
            llvm::Value* CodeGen(CodeGenContext& context);
    };

    //Minus Expression
    class MinusExpr : public Expression{
        public:
            Expression* Expr;
            MinusExpr(Expression* Expr) : Expr(Expr){};
            ~MinusExpr(){};
            llvm::Value* CodeGen(CodeGenContext& context);
    };

    //Not Expression
    class NotExpr : public Expression{
        public:
            Expression* Expr;
            NotExpr(Expression* Expr) : Expr(Expr){};
            ~NotExpr(){};
            llvm::Value* CodeGen(CodeGenContext& context);
    };

    //Exp [ Exp ]
    class ArrayVisitExpr : public Expression{
        public:
            Expression* Array;
            Expression* Index;
            ArrayVisitExpr(Expression* Array, Expression* Index):Array(Array), Index(Index){};
            ~ArrayVisitExpr(){};
            llvm::Value* CodeGen(CodeGenContext& context);
    };

    //INT
    class IntExpr : public Expression{
        public:
            int value;
            IntExpr(int value):value(value){};
            ~IntExpr(){};
            llvm::Value* CodeGen(CodeGenContext& context);
    };

    //FLOAT
    class FloatExpr : public Expression{
        public:
            float value;
            FloatExpr(float value):value(value){};
            ~FloatExpr(){};
            llvm::Value* CodeGen(CodeGenContext& context);
    };

    //ID
    class IDExpr : public Expression{
        public:
            std::string name;
            IDExpr(std::string str):name(str){};
            ~IDExpr(){};
            llvm::Value* CodeGen(CodeGenContext& context);
    };

    //Exp . ID
    class StructVisitExpr : public Expression{
        public:
            Expression* Struct;
            std::string name;
            StructVisitExpr(Expression* Struct, std::string name):Struct(Struct), name(name){};
            ~StructVisitExpr(){};
            llvm::Value* CodeGen(CodeGenContext& context);
    };
    
}