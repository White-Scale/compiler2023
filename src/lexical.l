
digit [0-9]
letter [a-zA-Z]
INT [+−]?[0-9]+
FLOAT [+−]?[0-9]*"."[0-9]+
ID [a-zA-Z][a-zA-Z_0-9]*


%%
";" {printf("SEMI");}
"," {printf("COMMA");}
"=" {printf("ASSIGNOP");}
">"|"<"|">="|"<="|"=="|"!=" {printf("RELOP");} 
"+" {printf("PLUS");}
"-" {printf("MINUS");}
"*" {printf("STAR");}
"/" {printf("/");}
"&&" {printf("AND");}
"||" {printf("OR");}
"." {printf("DOT");}
"!" {printf("NOT");}
"int"|"float"  {printf("TYPE");}
"(" {printf("LP");}
")" {printf("RP");}
"[" {printf("LB");}
"]" {printf("RB");}
"{" {printf("LC");}
"}" {printf("RC");}
"struct" {printf("STRUCT");}
"return" {printf("return");}
"if" {printf("IF");}
"else" {printf("ELSE");}
"while" {printf("WHILE");}
{FLOAT} {printf("FLOAT");}
{INT} {printf("INT");}
{ID} {printf("ID");}

%%
