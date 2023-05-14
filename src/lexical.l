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
">"|"<"|">="|"<="|"=="|"!=" {return RELOP;} 
"+" {return PLUS;}
"-" {return MINUS;}
"*" {return STAR;}
"/" {return DIV;}
"&&" {return AND;}
"||" {return OR;}
"." {return DOT;}
"!" {return NOT;}
"int"|"float"  {return TYPE;}
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
{FLOAT} {return FLOAT;}
{INT} {return INT;}
{ID} {return ID;}
" "|"\n" {;}
. {printf("Error type A at Line %d: Mysterious characters \'%s\'\n",yylineno, yytext);}
%%
