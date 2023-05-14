#include "AST.h"

void printTabs(int n)
{
    for (int i = 0; i < n; i++)
        printf("  ");
}

Node::Node(/* args */)
{
}

Node::~Node()
{
}

Program::Program(ExtDefList *ExtDefList)
{
    this->_ExtDefList = ExtDefList;
}

Program::~Program()
{
}

void Program::print(int n)
{
    printTabs(n);
    printf("Progarm->ExtDefList\n");
    for (auto v : *_ExtDefList)
    {
        v->print(n + 1);
    }
}

ExtDef::ExtDef(Specifier *_Specifier, ExtDecList *_ExtDecList, FunDec *_FunDec, CompSt *_CompSt)
{
    this->_Specifier = _Specifier;
    ;
    this->_ExtDecList = _ExtDecList;
    ;
    this->_FunDec = _FunDec;
    ;
    this->_CompSt = _CompSt;
    ;
}

ExtDef::~ExtDef()
{
}

void ExtDef::print(int n)
{
    printTabs(n);
    printf("ExtDef:\n");
    _Specifier->print(n + 1);
    
    if(_ExtDecList){
        printTabs(n + 1);
        printf("ExtDecList:\n");
        for (auto v : *_ExtDecList)
        {
            v->print(n + 2);
        }
    }
    if(_FunDec){
        _FunDec->print(n + 1);
        _CompSt->print(n + 1);
    }
}

Specifier::Specifier(Spec_type _Type, std::string *_Tag, DefList *_DefList)
{
    this->_Type = _Type;
    this->_Tag = _Tag;
    this->_DefList = _DefList;
}

Specifier::~Specifier()
{
}

void Specifier::print(int n)
{

    printTabs(n);
    printf("Specifier:\n");

    printTabs(n + 1);
    printf("type:%d\n", this->_Type);
    if (this->_Type == Spec_type::STRUCT_1)
    {
        printTabs(n + 1);
        if (_Tag)
        {
            printf("Tag:%s\n", this->_Tag->c_str());
        }
        else
        {
            printf("Tag:NULL\n");
        }
        if(_DefList){
            printTabs(n + 1);
            printf("DefList:\n");
            for (auto v : *_DefList)
            {
                v->print(n + 2);
            }
        }
    }
}

VarDec::VarDec(std::string *_Name, VarDec *_VarDec, int _Len)
{
    this->_Name = _Name;
    this->_VarDec = _VarDec;
    this->_Len = _Len;
}

VarDec::~VarDec()
{
}

void VarDec::print(int n)
{

    printTabs(n);
    printf("VarDec:\n");
    if (_Name)
    {
        printTabs(n + 1);
        printf("name:%s\n", this->_Name->c_str());
    }
    else
    {
        printTabs(n + 1);
        printf("length:%d\n", _Len);
        _VarDec->print(n + 1);
    }
}

FunDec::FunDec(std::string *_Name, VarList *_VarList)
{
    this->_Name = _Name;
    this->_VarList = _VarList;
}

FunDec::~FunDec()
{
}

void FunDec::print(int n)
{

    printTabs(n);
    printf("FunDec:\n");
    if (_Name)
    {
        printTabs(n + 1);
        printf("name:%s\n", this->_Name->c_str());
    }
    if (_VarList)
    {
        printTabs(n + 1);
        printf("VarList:\n");
        for (auto v : *_VarList)
        {
            v->print(n + 2);
        }
    }
}

ParamDec::ParamDec(Specifier *_Specifier, VarDec *_VarDec)
{
    this->_Specifier = _Specifier;
    this->_VarDec = _VarDec;
}

ParamDec::~ParamDec()
{
}

void ParamDec::print(int n)
{

    printTabs(n);
    printf("ParamDec:\n");
    _Specifier->print(n + 1);
    _VarDec->print(n + 1);
}

Stmt::Stmt(/* args */)
{
}

Stmt::~Stmt()
{
}

void Stmt::print(int n)
{

    printTabs(n);
    printf("Stmt:\n");
}


CompSt::CompSt(DefList *_DefList, StmtList *_StmtList)
{
    this->_StmtList = _StmtList;
    this->_DefList = _DefList;
}

CompSt::~CompSt()
{
}

void CompSt::print(int n)
{

    printTabs(n);
    printf("CompSt:\n");
    printTabs(n + 1);
    printf("DefList:\n");
    for (auto v : *_DefList)
    {
        v->print(n + 2);
    }
    printTabs(n + 1);
    printf("StmtList:\n");
    for (auto v : *_StmtList)
    {
        v->print(n + 2);
    }
}

Def::Def(Specifier *_Specifier, DecList *_DecList)
{
    this->_Specifier = _Specifier;
    this->_DecList = _DecList;
}

Def::~Def()
{
}

void Def::print(int n)
{

    printTabs(n);
    printf("Def:\n");
    _Specifier->print(n + 1);
    printTabs(n + 1);
    printf("DecList:\n");
    for (auto v : *_DecList)
    {
        v->print(n + 2);
    }
}

Dec::Dec(VarDec *_VarDec, Exp *_Exp)
{
    this->_VarDec = _VarDec;
    this->_Exp = _Exp;
}

Dec::~Dec()
{
}

void Dec::print(int n)
{

    printTabs(n);
    printf("Dec:\n");
    _VarDec->print(n + 1);
    if (_Exp)
    {
        _Exp->print(n + 1);
    }
}

Exp::Exp(/* args */)
{
}

Exp::~Exp()
{
}

void Exp::print(int n)
{
    printTabs(n);
    printf("Stmt:\n");
}