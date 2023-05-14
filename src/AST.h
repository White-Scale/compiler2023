#include <vector>

class ExtDef;

class Specifier;

class VarDec;
class FunDec;
class ParamDec;
typedef std::vector<ParamDec*> VarList;

class CompSt;
class Stmt;
typedef std::vector<Stmt*> StmtList;

class Def;
class Dec;
typedef std::vector<Def*> DefList;
typedef std::vector<Dec*> DecList;

class Exp;

class Args;

typedef std::vector<ExtDef*> ExtDefList;
typedef std::vector<VarDec*> ExtDecList;


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

class Program:public Node
{
private:
    /* data */
public:
    ExtDefList* _ExtDefList ;    
    Program(ExtDefList* _ExtDefList);
    ~Program();
};

Program::Program(ExtDefList* ExtDefList)
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
    Specifier * _Specifier;
    ExtDecList * _ExtDecList;
    FunDec * _FunDec;
    CompSt * _CompSt;
    ExtDef(Specifier * _Specifier,ExtDecList * _ExtDecList,FunDec * _FunDec,CompSt * _CompSt);
    ~ExtDef();
};

ExtDef::ExtDef(Specifier *_Specifier, ExtDecList *_ExtDecList, FunDec *_FunDec, CompSt *_CompSt)
{
    this->_Specifier=_Specifier;;
    this->_ExtDecList=_ExtDecList;;
    this->_FunDec=_FunDec;;
    this->_CompSt=_CompSt;;
}

ExtDef::~ExtDef()
{
}

class Specifier
{
private:
    /* data */
public:
    Specifier(/* args */);
    ~Specifier();
};

Specifier::Specifier(/* args */)
{
}

Specifier::~Specifier()
{
}

class VarDec
{
private:
    /* data */
public:
    VarDec(/* args */);
    ~VarDec();
};

VarDec::VarDec(/* args */)
{
}

VarDec::~VarDec()
{
}

class FunDec
{
private:
    /* data */
public:
    FunDec(/* args */);
    ~FunDec();
};

FunDec::FunDec(/* args */)
{
}

FunDec::~FunDec()
{
}

class ParamDec
{
private:
    /* data */
public:
    ParamDec(/* args */);
    ~ParamDec();
};

ParamDec::ParamDec(/* args */)
{
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
    CompSt(/* args */);
    ~CompSt();
};

CompSt::CompSt(/* args */)
{
}

CompSt::~CompSt()
{
}

class Def
{
private:
    /* data */
public:
    Def(/* args */);
    ~Def();
};

Def::Def(/* args */)
{
}

Def::~Def()
{
}

class Dec
{
private:
    /* data */
public:
    Dec(/* args */);
    ~Dec();
};

Dec::Dec(/* args */)
{
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

class Args
{
private:
    /* data */
public:
    Args(/* args */);
    ~Args();
};

Args::Args(/* args */)
{
}

Args::~Args()
{
}
