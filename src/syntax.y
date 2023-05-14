%{
 #include <stdio.h>
 #include <string>
 #include "AST.h"
 #include "lex.yy.cpp"
 Program * p;
 void yyerror(const char *s) { printf("ERROR: %s\n", s); }
%}

%union {
    Node *node;
    Program *program;
    ExtDef* extDef;
    ExtDefList* extDefList;
    Specifier* specifier;
    DefList* defList;
    VarDec* varDec;
    FunDec* funDec;
    ParamDec* paramDec;
    VarList* varList;
    CompSt* compSt;
    Stmt* stmt;
    StmtList* stmtList;
    Def* def;
    Dec* dec;
    DecList* decList;
    Exp* exp;
    Args* args;
    ExtDecList* extDecList;
    std::string *string;
    int Int;
    float Float;
    enum Spec_type Type;
}

%type <program> Program
%type <extDef> ExtDef
%type <extDefList> ExtDefList
%type <specifier> Specifier
%type <specifier> StructSpecifier 
%type <string> OptTag
%type <string> Tag
%type <varDec> VarDec
%type <funDec> FunDec
%type <paramDec> ParamDec
%type <varList> VarList
%type <compSt> CompSt
%type <stmt> Stmt
%type <stmtList> StmtList
%type <def> Def
%type <dec> Dec
%type <decList> DecList
%type <exp> Exp
%type <args> Args
%type <extDecList> ExtDecList

%type <defList> DefList

/* declared tokens */
%token <Int> INT
%token <Float> FLOAT
%token <string> ID
%token PLUS MINUS STAR DIV 
%token AND OR NOT DOT ASSIGNOP
%token RELOP
%token <Type> TYPE
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
Program     : ExtDefList                        {p = new Program($1);p->print(0);}
            ;                                   
ExtDefList  : ExtDef ExtDefList                 {$2->push_back($1);$$ = $2;}
            | /* empty */                       {$$ = new ExtDefList;}
            ;
ExtDef      : Specifier ExtDecList SEMI         {$$=new ExtDef($1,$2,NULL,NULL);}
            | Specifier SEMI                    {$$=new ExtDef($1,NULL,NULL,NULL);}
            | Specifier FunDec CompSt           {$$=new ExtDef($1,NULL,$2,$3);}
            ;
ExtDecList  : VarDec                            {$$ = new ExtDecList;$$->push_back($1);}
            | VarDec COMMA ExtDecList           {$3->push_back($1);$$ = $3;}
            ;
/* 7.1.3 Specifiers */
Specifier   : TYPE                              {printf("now we get a type,it is %d\n",$1);$$ = new Specifier($1,NULL,NULL);}
            | StructSpecifier                   {printf("now we get a type,it is a struct\n");$$=$1;}
            ; 
StructSpecifier : STRUCT OptTag LC DefList RC   {$$=new Specifier(Spec_type::STRUCT_1,$2,$4);}
            | STRUCT Tag                        {$$=new Specifier(Spec_type::STRUCT_1,$2,NULL);}
            ;
OptTag      : ID                                {$$=$1;}
            | /* empty */                       {$$=NULL;}
            ;
Tag         : ID                                {$$=$1;}
            ;
/* Declarators */                               
VarDec      : ID                                {$$=new VarDec($1,NULL,0);}
            | VarDec LB INT RB                  {$$=new VarDec(NULL,$1,$3);}
            ;        
FunDec      : ID LP VarList RP                  {$$=new FunDec($1,$3);}
            | ID LP RP                          {$$=new FunDec($1,NULL);}
            ;
VarList     : ParamDec COMMA VarList            {$3->push_back($1);$$ = $3;}
            | ParamDec                          {$$ = new VarList;$$->push_back($1);}
            ;
ParamDec    : Specifier VarDec                  {$$ = new ParamDec($1,$2);}
            ;
/* Statements */
CompSt      : LC DefList StmtList RC            {$$=new CompSt($2,$3);}
            | /* error recovery*/ error RC      {;}
            ;
StmtList    : Stmt StmtList                     {$2->push_back($1);$$ = $2;}
            | /* empty */                       {$$ = new StmtList;}
            ;
Stmt        : Exp SEMI                          {$$ = new Stmt();}
            | CompSt                            {$$ = new Stmt();}
            | RETURN Exp SEMI                   {$$ = new Stmt();}
            | IF LP Exp RP Stmt  %prec LOWER_THAN_ELSE  {$$ = new Stmt();}
            | IF LP Exp RP Stmt ELSE Stmt       {$$ = new Stmt();}
            | WHILE LP Exp RP Stmt              {$$ = new Stmt();}
            | /* error recovery*/ error SEMI    {$$ = new Stmt();}
            ;
/*  Local Definitions */
DefList     : Def DefList                       {$2->push_back($1);$$ = $2;}
            | /* empty */                       {$$ = new DefList;}
            ;
Def         : Specifier DecList SEMI            {$$ = new Def($1,$2);}
            ;
DecList     : Dec                               {$$ = new DecList;$$->push_back($1);}
            | Dec COMMA DecList                 {$3->push_back($1);$$ = $3;}
            ;
Dec         : VarDec                            {$$= new Dec($1,NULL);}
            | VarDec ASSIGNOP Exp               {$$= new Dec($1,$3);}
            ;
/* Expressions */
Exp         : Exp ASSIGNOP Exp                  {$$=new Exp();}
            | Exp AND Exp                       {$$=new Exp();}
            | Exp OR Exp                        {$$=new Exp();}
            | Exp RELOP Exp                     {$$=new Exp();}
            | Exp PLUS Exp                      {$$=new Exp();}
            | Exp MINUS Exp                     {$$=new Exp();}
            | Exp STAR Exp                      {$$=new Exp();}
            | Exp DIV Exp                       {$$=new Exp();}
            | LP Exp RP                         {$$=new Exp();}
            | MINUS Exp  %prec UMINUS           {$$=new Exp();}
            | NOT Exp                           {$$=new Exp();}
            | ID LP Args RP                     {$$=new Exp();}
            | ID LP RP                          {$$=new Exp();}
            | Exp LB Exp RB                     {$$=new Exp();}
            | Exp DOT ID                        {$$=new Exp();}
            | ID                                {$$=new Exp();}
            | INT                               {$$=new Exp();}
            | FLOAT                             {$$=new Exp();}
            | /* error recovery*/ error RP      {$$=new Exp();}
            ;
Args        : Exp COMMA Args                    {$3->push_back($1);$$ = $3;}
            | Exp                               {$$ = new Args;$$->push_back($1);}
            ;
%%

