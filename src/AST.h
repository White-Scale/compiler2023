#include <vector>
#include <string>

enum Spec_type
{
    INT_1,
    FLOAT_1,
    STRUCT_1
};

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

Node::Node(/* args */)
{
}

Node::~Node()
{
}

class Program : public Node
{
private:
    /* data */
public:
    ExtDefList *_ExtDefList;
    Program(ExtDefList *_ExtDefList);
    ~Program();
};

Program::Program(ExtDefList *ExtDefList)
{
    this->_ExtDefList = ExtDefList;
}

Program::~Program()
{
}

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
};

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
};

inline Specifier::Specifier(Spec_type _Type, std::string *_Tag, DefList *_DefList)
{
    this->_Type = _Type;
    this->_Tag = _Tag;
    this->_DefList = _DefList;
}

Specifier::~Specifier()
{
}

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
};

VarDec::VarDec(std::string *_Name, VarDec *_VarDec, int _Len)
{
    this->_Name = _Name;
    this->_VarDec = _VarDec;
    this->_Len = _Len;
}

VarDec::~VarDec()
{
}

class FunDec
{
private:
    /* data */
public:
    std::string *_Name;
    VarList *_VarList;
    FunDec(std::string *_Name, VarList *_VarList);
    ~FunDec();
};

FunDec::FunDec(std::string *_Name, VarList *_VarList)
{
    this->_Name = _Name;
    this->_VarList = _VarList;
}

FunDec::~FunDec()
{
}

class ParamDec
{
private:
    /* data */
public:
    Specifier *_Specifier;
    VarDec *_VarDec;
    ParamDec(Specifier *_Specifier, VarDec *_VarDec);
    ~ParamDec();
};

ParamDec::ParamDec(Specifier *_Specifier, VarDec *_VarDec)
{
    this->_Specifier = _Specifier;
    this->_VarDec = _VarDec;
}

ParamDec::~ParamDec()
{
}

class Stmt
{
private:
    /* data */
public:
    Stmt(/* args */);
    ~Stmt();
};

Stmt::Stmt(/* args */)
{
}

Stmt::~Stmt()
{
}

class CompSt
{
private:
    /* data */
public:
    StmtList *_StmtList;
    DefList *_DefList;
    CompSt(DefList *_DefList, StmtList *_StmtList);
    ~CompSt();
};

CompSt::CompSt(DefList *_DefList, StmtList *_StmtList)
{
    this->_StmtList = _StmtList;
    this->_DefList = _DefList;
}

CompSt::~CompSt()
{
}

class Def
{
private:
    /* data */
public:
    Specifier *_Specifier;
    DecList *_DecList;
    Def(Specifier *_Specifier, DecList *_DecList);
    ~Def();
};

Def::Def(Specifier *_Specifier, DecList *_DecList)
{
    this->_Specifier = _Specifier;
    this->_DecList = _DecList;
}

Def::~Def()
{
}

class Dec
{
private:
    /* data */
public:
    VarDec *_VarDec;
    Exp *_Exp;
    Dec(VarDec *_VarDec, Exp *_Exp);
    ~Dec();
};

Dec::Dec(VarDec *_VarDec, Exp *_Exp)
{
    this->_VarDec = _VarDec;
    this->_Exp = _Exp;
}

Dec::~Dec()
{
}

class Exp
{
private:
    /* data */
public:
    Exp(/* args */);
    ~Exp();
};

Exp::Exp(/* args */)
{
}

Exp::~Exp()
{
}
