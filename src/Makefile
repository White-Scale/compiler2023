CC=gcc
syntax.tab.h:
	bison -d -v syntax.y
lex.yy.c: lexical.l syntax.tab.h
	flex lexical.l
# scanner: main.c lex.yy.c
# 	$(CC) main.c lex.yy.c -lfl -o scanner
syntax.tab.c: syntax.y lex.yy.c
	bison -d syntax.y
parser: main.c syntax.tab.c 
	$(CC) main.c syntax.tab.c -lfl -ly -o parser
