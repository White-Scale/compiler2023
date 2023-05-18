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
"int"|"float"  {if(strcmp(yytext,"int"))yylval.typeId=AST::BasicType::TypeID::_int;
                else yylval.typeId=AST::BasicType::TypeID::_float; 
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
