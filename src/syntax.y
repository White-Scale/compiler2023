%{
 #include "AST.hpp"
 #include <stdio.h>
 #include <string>
 #include "lex.yy.cpp"
AST::Program * p;


//  extern void yyerror(const char *s);
 void yyerror(const char *s) { printf("ERROR: %s\n", s); }
%}


%union {
    AST::Program * program;
    AST::ExtDefList * extDefList;
    AST::Expression * expression;
    AST::Declaration * declaration;
    AST::VarDec * varDec;
    AST::VarType * varType;
    AST::BasicType * basicType;
    AST::ArrayType * arrayType;
    AST::VarDecList * varDecList;
    AST::VarList *  varList;    
    AST::VarInit *  varInit;
    AST::FunDec *   funDec;
    AST::ArgList *  argList;
    AST::Arg *      arg;
    AST::CompStmt * compStmt;
    AST::StmtList * stmtList;
    AST::Statement *     stmt;
    AST::Args *     args;
    AST::BasicType::TypeID typeId;
    std::string * string;
    int Int;
    float Float;
}

%type <expression> Exp;
%type <program> Program;
%type <extDefList> ExtDefList;
%type <declaration> ExtDef;
/* %type <basicType> BasicType; */
%type <arrayType> ArrayType;
%type <varDec> VarDec;
%type <varType> VarType;
%type <varDecList> VarDecList;
%type <varList>     VarList;
%type <varInit>     VarInit;
%type <funDec>      FunDec;
%type <argList>     ArgList;
%type <arg>         Arg;
%type <compStmt>    CompStmt;
%type <stmtList>    StmtList;
%type <stmt>        Stmt;
%type <args>        Args;

/* declared tokens */
%token <Int>INT
%token <Float>FLOAT
%token <string> ID
%token PLUS MINUS STAR DIV 
%token AND OR NOT DOT ASSIGNOP
%token <string> RELOP
%token <typeId> TYPE
%token LP RP LB RB LC RC
%token STRUCT RETURN IF ELSE WHILE
%token SEMI COMMA
%token UMINUS /* use for the "negative" oprator */

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right UMINUS NOT
%left LP RP LB RB DOT

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE



%%
/* High-level Definitions */
Program     : ExtDefList                        {p = new AST::Program($1);}
            ;                                   
ExtDefList  : ExtDef ExtDefList                 {$2->push_back($1);$$ = $2;}
            | /* empty */                       {$$ = new AST::ExtDefList();}
            ;
ExtDef      : VarDec                            {$$ = $1;}
            | FunDec                            {$$ = $1;}
            ; 
/* Types*/
VarType     : TYPE      {$$ = new AST::BasicType($1);}
            | ArrayType {$$ = $1;}
            /*|STructType */ 
            ;
ArrayType   : VarType LB INT RB {$$ = new AST::ArrayType($1,$3);}
            ;
/* Declarations */                               
VarDec      : VarType VarList SEMI              {$$ = new AST::VarDec($1,$2);}
            ; 

VarList     : VarInit COMMA VarList             {$3->push_back($1);$$ = $3;}
            | VarInit                           {$$ = new AST::VarList();$$->push_back($1);}
            ;
VarInit     : ID                                {$$ = new AST::VarInit(*$1,NULL);}
            | ID ASSIGNOP Exp                   {$$ = new AST::VarInit(*$1,$3);}
            /* Exp should be constant*/
            ;

FunDec      : VarType ID LP ArgList RP CompStmt         {$$ = new AST::FunDec($1,*$2,$4,$6);delete $2;}
            | VarType ID LP RP CompStmt                 {$$ = new AST::FunDec($1,*$2,NULL,$5);delete $2;}
            ;
ArgList     : ArgList COMMA Arg                 {$1->push_back($3);$$=$1;}
            | Arg                               {$$ = new AST::ArgList();$$->push_back($1);}
            ;
Arg         : VarType ID
            ;
/* Statements */
CompStmt    : LC VarDecList StmtList RC         {$$ = new AST::CompStmt($2,$3);}
            | /* error recovery*/ error RC      {;}
            ;
StmtList    : Stmt StmtList                     {$2->push_back($1);$$=$2;}
            | /* empty */                       {$$=new AST::StmtList();}
            ;
VarDecList  : VarDec VarDecList                 {$2->push_back($1);$$=$2;}
            | /* empty */                       {$$=new AST::VarDecList();}

Stmt        : Exp SEMI                          {$$ = new AST::ExpStmt($1);}
            | CompStmt                          {$$ = $1;}
            | RETURN Exp SEMI                   {$$ = new AST::RetStmt($2) ;}
            | IF LP Exp RP Stmt  %prec LOWER_THAN_ELSE  {$$ = new AST::IfStmt($3,$5,NULL);}
            | IF LP Exp RP Stmt ELSE Stmt       {$$ = new AST::IfStmt($3,$5,$7);}
            | WHILE LP Exp RP Stmt              {$$ = new AST::WhileStmt($3,$5);}
            | /* error recovery*/ error SEMI    {;}
            ;
/* Expressions */
Exp         : Exp ASSIGNOP Exp                  {$$ = new AST::AssignOpExpr($1,$3);}
            | Exp AND Exp                       {$$ = new AST::BinaryOpExpr("&",$1,$3);}
            | Exp OR Exp                        {$$ = new AST::BinaryOpExpr("|",$1,$3);}
            | Exp RELOP Exp                     {$$ = new AST::BinaryOpExpr(*$2,$1,$3);delete $2;}
            | Exp PLUS Exp                      {$$ = new AST::BinaryOpExpr("+",$1,$3);}
            | Exp MINUS Exp                     {$$ = new AST::BinaryOpExpr("-",$1,$3);}
            | Exp STAR Exp                      {$$ = new AST::BinaryOpExpr("*",$1,$3);}
            | Exp DIV Exp                       {$$ = new AST::BinaryOpExpr("/",$1,$3);}
            | LP Exp RP                         {$$  =$2;}/* no sure if it is correct*/
            | MINUS Exp  %prec UMINUS           {$$ = new AST::MinusExpr($2);}
            | NOT Exp                           {$$ = new AST::NotExpr($2);}
            | ID LP Args RP                     {$$ = new AST::CallFuncExpr(*$1,$3);delete $1;}
            | ID LP RP                          {$$ = new AST::CallFuncExpr(*$1,NULL);delete $1;}
            | Exp LB Exp RB                     {$$ = new AST::ArrayVisitExpr($1,$3);}
            | Exp DOT ID                        {$$ = new AST::StructVisitExpr($1,*$3);} /* struct not finsh*/
            | ID                                {$$ = new AST::IDExpr(*$1);delete $1;}
            | INT                               {$$ = new AST::IntExpr($1);}
            | FLOAT                             {$$ = new AST::FloatExpr($1);}
            | /* error recovery*/ error RP      {;}
            ;
Args        : Exp COMMA Args                    {$3->push_back($1);$$=$3;}
            | Exp                               {$$ = new AST::Args();$$->push_back($1);}
            ;
%%

