%{
 #include <stdio.h>
 #include <string>
 #include "AST.h"
 #include "lex.yy.cpp"
 void yyerror(const char *s) { printf("ERROR: %s\n", s); }
%}

%union {
    Node *node;
    std::string *string;
    int Int;
}

/* declared tokens */
%token <Int> INT
%token FLOAT
%token <string> ID
%token PLUS MINUS STAR DIV 
%token AND OR NOT DOT ASSIGNOP
%token RELOP
%token TYPE
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
Program     : ExtDefList                        {;}
            ;                                   
ExtDefList  : ExtDef ExtDefList                 {;}
            | /* empty */                       {;}
            ;
ExtDef      : Specifier ExtDecList SEMI         {;}
            | Specifier SEMI                    {;}
            | Specifier FunDec CompSt           {;}
            ;
ExtDecList  : VarDec                            {;}
            | VarDec COMMA ExtDecList           {;}
            ;
/* 7.1.3 Specifiers */
Specifier   : TYPE                              {;}
            | StructSpecifier                   {;}
            ; 
StructSpecifier : STRUCT OptTag LC DefList RC   {;}
            | STRUCT Tag                        {;}
            ;
OptTag      : ID                                {;}
            | /* empty */                       {;}
            ;
Tag         : ID                                {;}
            ;
/* Declarators */                               
VarDec      : ID                                {;}
            | VarDec LB INT RB                  {;}
            ;        
FunDec      : ID LP VarList RP                  {;}
            | ID LP RP                          {;}
            ;
VarList     : ParamDec COMMA VarList            {;}
            | ParamDec                          {;}
            ;
ParamDec    : Specifier VarDec                  {;}
            ;
/* Statements */
CompSt      : LC DefList StmtList RC            {;}
            | /* error recovery*/ error RC      {;}
            ;
StmtList    : Stmt StmtList                     {;}
            | /* empty */                       {;}
            ;
Stmt        : Exp SEMI                          {;}
            | CompSt                            {;}
            | RETURN Exp SEMI                   {;}
            | IF LP Exp RP Stmt  %prec LOWER_THAN_ELSE  {;}
            | IF LP Exp RP Stmt ELSE Stmt       {;}
            | WHILE LP Exp RP Stmt              {;}
            | /* error recovery*/ error SEMI    {;}
            ;
/*  Local Definitions */
DefList     : Def DefList                       {;}
            | /* empty */                       {;}
            ;
Def         : Specifier DecList SEMI            {;}
            ;
DecList     : Dec                               {;}
            | Dec COMMA DecList                 {;}
            ;
Dec         : VarDec                            {;}
            | VarDec ASSIGNOP Exp               {;}
            ;
/* Expressions */
Exp         : Exp ASSIGNOP Exp                  {;}
            | Exp AND Exp                       {;}
            | Exp OR Exp                        {;}
            | Exp RELOP Exp                     {;}
            | Exp PLUS Exp                      {;}
            | Exp MINUS Exp                     {;}
            | Exp STAR Exp                      {;}
            | Exp DIV Exp                       {;}
            | LP Exp RP                         {;}
            | MINUS Exp  %prec UMINUS           {;}
            | NOT Exp                           {;}
            | ID LP Args RP                     {;}
            | ID LP RP                          {;}
            | Exp LB Exp RB                     {;}
            | Exp DOT ID                        {;}
            | ID                                {;}
            | INT                               {;}
            | FLOAT                             {;}
            | /* error recovery*/ error RP      {;}
            ;
Args        : Exp COMMA Args                    {;}
            | Exp                               {;}
            ;
%%

