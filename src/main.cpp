#include <stdio.h>
#include "AST.hpp"
#include "CodeGenContext.hpp"
// #define yylex as extern "C"
FILE *f;
// namespace yyFlexLexer{};
extern "C" int yylex();
void yyrestart(FILE *input_file);
int yyparse(void);
void yyerror(const char *s);

extern AST::Program * p;

int main(int argc, char **argv)
{
    if (argc <= 1)
        return 1;
    FILE *f = fopen(argv[1], "r");
    if (!f)
    {
        perror(argv[1]);
        return 1;
    }
    ::yyrestart(f);
    ::yyparse();
    CodeGenContext cgc;
    p->CodeGen(cgc);
    cgc.module->print(llvm::outs(),nullptr);
    return 0;
}
