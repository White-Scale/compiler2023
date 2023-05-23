%{
#include "syntax.tab.hpp"   
%}
%option yylineno
%option noyywrap 
digit [0-9]
letter [a-zA-Z]
/* INT [+−]?[0-9]+ */
/* FLOAT [+−]?[0-9]*"."[0-9]+ */
INT [0-9]+
FLOAT [+−]?[0-9]*"."[0-9]+
ID [a-zA-Z][a-zA-Z_0-9]*
/*STRING "\"" */



%%
";" {return SEMI;}
"," {return COMMA;}
"=" {return ASSIGNOP;}
">"|"<"|">="|"<="|"=="|"!=" {yylval.string = new std::string(yytext);return RELOP;} 
"+" {return PLUS;}
"-" {return MINUS;}
"*" {return STAR;}
"/" {return DIV;}
"&&" {return AND;}
"||" {return OR;}
"." {return DOT;}
"!" {return NOT;}
"int"|"float"|"char"  {if(strcmp(yytext,"int")==0)yylval.typeId=AST::BasicType::TypeID::_int;
                else if (strcmp(yytext,"float")==0)yylval.typeId=AST::BasicType::TypeID::_float;
                else yylval.typeId=AST::BasicType::TypeID::_char;
                return TYPE;}
"(" {return LP;}
")" {return RP;}
"[" {return LB;}
"]" {return RB;}
"{" {return LC;}
"}" {return RC;}
"struct" {return STRUCT;}
"return" {return RETURN;}
"if" {return IF;}
"else" {return ELSE;}
"while" {return WHILE;}
{FLOAT} {yylval.Float = atof(yytext);return FLOAT;}
{INT} {yylval.Int = atoi(yytext);return INT;}
{ID} {yylval.string = new std::string(yytext);return ID;}
" "|"\n" {;}
. {printf("Error type A at Line %d: Mysterious characters \'%s\'\n",yylineno, yytext);}
%%
