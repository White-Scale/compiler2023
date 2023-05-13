CC=gcc
lex.yy.c: lexical.l
	flex lexical.l
scanner: main.c lex.yy.c
	$(CC) main.c lex.yy.c -lfl -o scanner