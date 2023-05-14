#include <vector>
#include <string>

enum Spec_type
{
    INT_1,
    FLOAT_1,
    STRUCT_1
};

void printTabs(int n);

class ExtDef;

class Specifier;

class VarDec;
class FunDec;
class ParamDec;
typedef std::vector<ParamDec *> VarList;

class CompSt;
class Stmt;
typedef std::vector<Stmt *> StmtList;

class Def;
class Dec;
typedef std::vector<Def *> DefList;
typedef std::vector<Dec *> DecList;

class Exp;

typedef std::vector<Exp *> Args;

typedef std::vector<ExtDef *> ExtDefList;
typedef std::vector<VarDec *> ExtDecList;

class Node
{
private:
    /* data */
public:
    Node(/* args */);
    ~Node();
};

class Program
{
private:
    /* data */
public:
    ExtDefList *_ExtDefList;
    Program(ExtDefList *_ExtDefList);
    ~Program();
    void print(int n);
};

class ExtDef
{
private:
    /* data */
public:
    Specifier *_Specifier;
    ExtDecList *_ExtDecList;
    FunDec *_FunDec;
    CompSt *_CompSt;
    ExtDef(Specifier *_Specifier, ExtDecList *_ExtDecList, FunDec *_FunDec, CompSt *_CompSt);
    ~ExtDef();
    void print(int n);
};

class Specifier
{
private:
    /* data */
public:
    Spec_type _Type;
    std::string *_Tag;
    DefList *_DefList;
    Specifier(Spec_type _Type, std::string *_Tag, DefList *_DefList);
    ~Specifier();
    void print(int n);
};

class VarDec
{
private:
    /* data */
public:
    std::string *_Name;
    VarDec *_VarDec;
    int _Len;
    VarDec(std::string *_Name, VarDec *_VarDec, int _Len);
    ~VarDec();
    void print(int n);
};

class FunDec
{
private:
    /* data */
public:
    std::string *_Name;
    VarList *_VarList;
    FunDec(std::string *_Name, VarList *_VarList);
    ~FunDec();
    void print(int n);
};

class ParamDec
{
private:
    /* data */
public:
    Specifier *_Specifier;
    VarDec *_VarDec;
    ParamDec(Specifier *_Specifier, VarDec *_VarDec);
    ~ParamDec();
    void print(int n);
};

class Stmt
{
private:
    /* data */
public:
    Stmt(/* args */);
    ~Stmt();
    void print(int n);
};

class CompSt
{
private:
    /* data */
public:
    StmtList *_StmtList;
    DefList *_DefList;
    CompSt(DefList *_DefList, StmtList *_StmtList);
    ~CompSt();
    void print(int n);
};

class Def
{
private:
    /* data */
public:
    Specifier *_Specifier;
    DecList *_DecList;
    Def(Specifier *_Specifier, DecList *_DecList);
    ~Def();
    void print(int n);
};

class Dec
{
private:
    /* data */
public:
    VarDec *_VarDec;
    Exp *_Exp;
    Dec(VarDec *_VarDec, Exp *_Exp);
    ~Dec();
    void print(int n);
};

class Exp
{
private:
    /* data */
public:
    Exp(/* args */);
    ~Exp();
    void print(int n);
};
