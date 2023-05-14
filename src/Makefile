CC=g++
syntax.tab.hpp:
	bison -d -v syntax.y
lex.yy.cpp: lexical.l syntax.tab.hpp
	flex -o lex.yy.cpp lexical.l 
# scanner: main.c lex.yy.c
# 	$(CC) main.c lex.yy.c -lfl -o scanner
syntax.tab.cpp: syntax.y lex.yy.cpp
	bison -d syntax.y -o syntax.tab.cpp
parser: main.cpp syntax.tab.cpp 
# $(CC) main.cpp syntax.tab.cpp lex.yy.cpp -lfl -ly -o parser
	$(CC) -g main.cpp syntax.tab.cpp -lfl -ly -o parser
