CC=g++
LLVM_flags = `llvm-config --libs core mcjit native --cxxflags --ldflags` 
# -w  -DLLVM_DISABLE_ABI_BREAKING_CHECKS_ENFORCING=1
syntax.tab.hpp: syntax.tab.cpp
# bison -d -v syntax.y
lex.yy.cpp: lexical.l syntax.tab.hpp
	flex -o lex.yy.cpp lexical.l 
# scanner: main.c lex.yy.c
# 	$(CC) main.c lex.yy.c -lfl -o scanner
syntax.tab.cpp: syntax.y lex.yy.cpp
	bison  -v -d syntax.y -o syntax.tab.cpp
parser: main.cpp syntax.tab.cpp 
# $(CC) main.cpp syntax.tab.cpp lex.yy.cpp -lfl -ly -o parser
	$(CC) `llvm-config --libs core mcjit native --cxxflags --ldflags` \
	 main.cpp syntax.tab.cpp AST.cpp CodeGenContext.cpp -lfl -ly   \
	 `llvm-config --libs core mcjit native --cxxflags --ldflags` -g -o parser
