CC=g++
LLVM_flags = `llvm-config --libs core mcjit native --cxxflags --ldflags` 

default: 
	make cmmc
	rm -f syntax.tab.cpp syntax.tab.hpp lex.yy.cpp syntax.output 

# cmmc means c minus minus compiler
cmmc: main.cpp syntax.tab.cpp 
	$(CC) $(LLVM_flags) \
	 main.cpp syntax.tab.cpp AST.cpp CodeGenContext.cpp visualization.cpp -lfl -ly   \
	 $(LLVM_flags)  -g -o cmmc

syntax.tab.hpp: syntax.tab.cpp
lex.yy.cpp: lexical.l syntax.tab.hpp
	flex -o lex.yy.cpp lexical.l 
syntax.tab.cpp: syntax.y lex.yy.cpp
	bison  -v -d syntax.y -o syntax.tab.cpp

clean:
	rm -f cmmc syntax.tab.cpp syntax.tab.hpp lex.yy.cpp syntax.output *.ll *.bc *.out ../test/*.ll ../test/*.bc ../test/*.out