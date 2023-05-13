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
            class VarInit : public VarDec;
            using VarList = std::vector<VarInit*>;
        class FunDec : public Declaration;
            class Arg : public FunDec;
            class ArgList : public std::vector<Arg*>;
            class FunBody public FunDec;
        class TypeDec : public Declaration;

    class VarType : public Node;
        class BasicType : public VarType;   //INT, FLOAT
        class ArrayType : public VarType;   //VarType LB INT RB
        class StructType : public VarType;  //STRUCT Tag
            class FieldDec : public StructType; //Specifier VarDecList SEMI
            using FieldList = std::vector<std::string>;

    class Statement : public Node;
        class CompStmt : public Statement;  //LC DefList StmtList RC, Statement Blocks
        class IfStmt : public Statement;    //IF LP Exp RP Stmt
        class RetStmt : public Statement;   //RETURN Exp SEMI
        // class IfElseStmt : public Statement;    //IF LP Exp RP Stmt ELSE Stmt
        class WhileStmt : public Statement; //WHILE LP Exp RP Stmt
        class ForStmt : public Statement;   //FOR LP Exp SEMI Exp SEMI Exp RP Stmt

    // class Definition : public Node;     //Def DefList

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

namespace AST {

    //Node interface for AST nodes
    class Node {
        public:
            Node(){}
            ~Node() {}
            virtual llvm::Value* CodeGen() = 0;
    };

    //Program Interface
    class Program : public Node {
        public:
            std::vector<Declaration*> _Decs;
            Program(std::vector<Declaration*> _Decs):_Decs(_Decs){};
            ~Program(){};
            llvm::Value* CodeGen();
    };
    
    //Declaration Interface
    class Declaration : public Node {
        public:
            Declaration(){}
            ~Declaration(){}
            virtual llvm::Value* CodeGen() = 0;
    };

    //Varaible Declaration
    class VarDec : public Declaration {
        public:
            VarType* _VarType;
            VarList* _VarList;
            VarDec(VarType* _VarType, VarList* _VarList):_VarType(_VarType), _VarList(_VarList){};
            ~VarDec(){};
            llvm::Value* CodeGen();
    };

    //a variable declaration with initialization
    class VarInit : public VarDec {
        public:
            std::string _name;      //variable name
            Expression* _value;      // possible value
            VarInit(std::string _name, Expression* _value):_name(_name), _value(_value){};
            ~VarInit(){};
            llvm::Value* CodeGen() {return NULL};
    };

    //Function Declaration
    class FunDec : public Declaration {
        public:
            VarType* _returnType;   //return type
            std::string _name;      //function name
            ArgList* _args;         //arguments
            FunBody* _body;         //function body
            FunDec(VarType* _returnType, std::string _name, ArgList* _args, FunBody* _body):_returnType(_returnType), _name(_name), _args(_args), _body(_body){};
            ~FunDec(){};
            llvm::Value* CodeGen();
    };  

    //Function argument
    class Arg : public FunDec {
        public:
            VarType* _type;
            std::string _name;
            Arg(VarType* _type, std::string _name):_type(_type), _name(_name){};
            ~Arg(){};
            llvm::Value* CodeGen() {return NULL};
    };

    //Function argument list
    class ArgList : public std::vector<Arg*> {
        public:
            ArgList(){};
            ~ArgList(){};
            llvm::Value* CodeGen() {return NULL};
    };

    //Function body
    class FunBody : public FunDec {
        public:
            CompStmt* _compStmt;
            FunBody(CompStmt* _compStmt):_compStmt(_compStmt){};
            ~Funbody(){};
            llvm::Value* CodeGen();
    };

    //Type Declaration
    class TypeDec : public Declaration {
        public:
            VarType* _type;
            TypeDec(VarType* _type):_type(_type){};
            ~TypeDec(){};
            llvm::Value* CodeGen();
    };

    //Base class for variable type
    class VarType : public Node {
        public:
            llvm::Type* _type;
            VarType(void):_type(NULL){}
            ~VarType(){}
            virtual llvm::Type* GetType() = 0;
            virtual llvm::Value* CodeGen() {return NULL};
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
                _clar,
                _void,
                _bool
            }
            TypeID _type;
            BasicType(TypeID _type):_type(_type){};
            ~BasicType(){};
            llvm::Type* GetType();
            bool isBasicType() {return true};
            bool isArrayType() {return false};
            bool isStructType() {return false};
    }

    //Array Type
    class ArrayType : public VarType {
        public:
            VarType* _type;
            int _size;
            ArrayType(VarType* _type, int _size):_type(_type), _size(_size){};
            ArrayType(VarType* _type):_type(_type), _size(0){};     //not initialized
            ~ArrayType(){};
            llvm::Type* GetType();
            bool isBasicType() {return false};
            bool isArrayType() {return true};
            bool isStructType() {return false};
    }

    //Struct Type
    class StructType : public VarType {
        FieldDec* _structbody;      //each field in struct body
        StructType(FieldDec* _structbody):_structbody(_structbody){};
        ~StructType(){};
        llvm::Type* GetType();
        llvm::Type* GenerateStruct(CodeGenerator& __Generator, "<unnamed>");  //generate empty struct
        llvm::Type* GenerateBody(CodeGenerator& __Generator);
        bool isBasicType() {return false};
        bool isArrayType() {return false};
        bool isStructType() {return true};
    }

    //Field declaration in struct
    class FieldDec : public StructType {
        public:
            VarType* _type;
            FieldList* _fieldlist;
            FieldDec(VarType* _type, FieldList* _fieldlist):_type(_type), _fieldlist(_fieldlist){};
            ~FieldDec(){};
            llvm::Value* CodeGen() {return NULL};
    }

    //Statement Interface
    class Statement : public Node {
        public:
            Statement(){}
            ~Statement(){}
            virtual llvm::Value* CodeGen() = 0;
    };

    //Compound Statement
    class CompStmt : public Statement {
        public:
            Statement* _stmt;
            CompStmt(Statement* _stmt):_stmt(_stmt){};
            ~CompStmt(){};
            llvm::Value* CodeGen();
    }

    //If Statement
    class IfStmt : public Statement {
        public:
            Expression* _cond;
            Statement* _then;
            Statement* _else;
            IfStmt(Expression* _cond, Statement* _then, Statement* _else = NULL):_cond(_cond), _then(_then), _else(_else){};
            ~IfStmt(){};
            llvm::Value* CodeGen();
    }

    //Return Statement
    class RetStmt : public Statement {
        public:
            Expression* _retval;
            RetStmt(Expression* _retval):_retval(_retval){};
            ~RetStmt(){};
            llvm::Value* CodeGen();
    }

    //While Statement
    class WhileStmt : public Statement {
        public:
            Expression* _cond;
            Statement* _body;
            WhileStmt(Expression* _cond, Statement* _body):_cond(_cond), _body(_body){};
            ~WhileStmt(){};
            llvm::Value* CodeGen();
    }

    //For Statement
    class ForStmt : public Statement {
        public:
            Expression* _init;
            Expression* _cond;
            Expression* _step;
            Statement* _body;
            ForStmt(Expression* _init, Expression* _cond, Expression* _step, Statement* _body):_init(_init), _cond(_cond), _step(_step), _body(_body){};
            ~ForStmt(){};
            llvm::Value* CodeGen();
    }

    //Expression Interface
    class Expression : public Node {
        public:
            Expression(){}
            ~Expression(){}
            virtual llvm::Value* CodeGen() = 0;
    }

    //Exp ASSIGN Exp
    class AssignOpExpr : public Expression {
        public:
            Expression* LHS;
            Expression* RHS;
            AssignOpExpr(Expression* LHS, Expression* RHS):LHS(LHS), RHS(RHS){};
            ~AssignOpExpr(){};
            llvm::Value* CodeGen();
    }

    //Exp BinOP RHS
    class BinaryOpExpr : public Expression {
        public:
            char Operator;//AND\OR\RELOP\PLUS...
            Expression* LHS;
            Expression* RHS;
            BinaryOpExpr(char Operator, Expression* LHS, Expression* RHS):Operator(Operator), LHS(LHS), RHS(RHS){};
            ~BinaryOpExpr() {};
            llvm::Value* CodeGen();
    }

    //Minus Expression
    class MinusExpr : public Expression {
        public:
            Expression* Expr;
            MinusExpr(Expression* Expr) : Expr(Expr){};
            ~MinusExpr(){};
            llvm::Value* CodeGen();
    }

    //Not Expression
    class NotExpr : public Expression {
        public:
            Expression* Expr;
            NotExpr(Expression* Expr) : Expr(Expr){};
            ~NotExpr(){};
            llvm::Value* CodeGen();
    }

    //Exp [ Exp ]
    class ArrayVisitExpr : public Expression {
        public:
            Expression* Array;
            Expression* Index;
            ArrayVisitExpr(Expression* Array, Expression* Index):Array(Array), Index(Index){};
            ~ArrayVisitExpr(){};
            llvm::Value* CodeGen();
    }

    //INT
    class IntExpr : public Expression {
        public:
            int value;
            IntExpr(int value):value(value){};
            ~IntExpr(){};
            llvm::Value* CodeGen();
    }

    //FLOAT
    class FloatExpr : public Expression {
        public:
            float value;
            FloatExpr(float value):value(value){};
            ~FloatExpr(){};
            llvm::Value* CodeGen();
    }

    //ID
    class IDExpr : public Expression {
        public:
            std::string name;
            IDExpr(std::string str):name(str){};
            ~IDExpr(){};
            llvm::Value* CodeGen();
    }

    //Exp . ID
    class StructVisitExpr : public Expression {
        public:
            Expression* Struct;
            std::string name;
            StructVisitExpr(Expression* Struct, std::string name):Struct(Struct), name(name){};
            ~StructVisitExpr(){};
            llvm::Value* CodeGen();
    }
    
}