#include <stdio.h>
#include <algorithm>
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



std::string replaceExtensionWithBC(const std::string& filename) {
    std::string newFilename = filename;
    size_t dotPos = newFilename.find_last_of('.');
    size_t slashPos = newFilename.find_last_of('/');
    size_t backslashPos = newFilename.find_last_of('\\');
    size_t separatorPos = std::max(slashPos, backslashPos);

    if (dotPos != std::string::npos && (dotPos > separatorPos || separatorPos == std::string::npos)) {
        newFilename = newFilename.substr(0, dotPos) + ".bc";
    } else {
        newFilename += ".bc";
    }
    return newFilename;
}

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
    std::string inputFilename = std::string(argv[1]);
    std::string outputFilename = replaceExtensionWithBC(inputFilename);
    std::error_code errorCode;
    llvm::raw_fd_ostream outputFile(outputFilename, errorCode, llvm::sys::fs::OF_None);
    if (errorCode) {
        std::cout << "cantnot open out put file";
        return 1;
    }
    llvm::WriteBitcodeToFile(*(cgc._module), outputFile);
    outputFile.close();
    // cgc._module->print(llvm::outs(),nullptr);
    return 0;
}
