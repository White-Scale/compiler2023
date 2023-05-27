#include"AST.hpp"
#include <stdio.h>
#include <algorithm>
#include <regex>
#include <fstream>
#include <unordered_map>

using namespace AST;
std::unordered_map<AST::Node*, std::string> nodeIDs;

std::vector<Node*> Program::getChildren() {
    std::vector<Node*> children;
    for (auto& dec : *(this->_decs)) {
        children.push_back(dec);
    }
    return children;
}

std::vector<Node*> VarDec::getChildren() {
    std::vector<Node*> children;
    if (_VarType)
        children.push_back(_VarType);
    if (_VarList) {
        for (auto& var : *_VarList) {
            children.push_back(var);
        }
    }
    return children;
}

std::vector<Node*> VarInit::getChildren() {
    return std::vector<Node*>();
}

std::vector<Node*> FunDec::getChildren() {
    std::vector<Node*> children;
    if (_returnType)
        children.push_back(_returnType);
    if (_args) {
        for (auto& arg : *_args) {
            children.push_back(arg);
        }
    }
    if (_body)
        children.push_back(_body);
    return children;
}

std::vector<Node*> Arg::getChildren() {
    return std::vector<Node*>();
}

std::vector<Node*> TypeDec::getChildren() {
    std::vector<Node*> children;
    if (_type)
        children.push_back(_type);
    return children;
}

std::vector<Node*> BasicType::getChildren() {
    return std::vector<Node*>();
}

std::vector<Node*> ArrayType::getChildren() {
    std::vector<Node*> children;
    children.push_back(_type);
    return children;
}

std::vector<Node*> CompStmt::getChildren() {
    std::vector<Node*> children;
    if (_stmts) {
        for (auto stmt : *_stmts) {
            children.push_back(stmt);
        }
    }
    if (_varDecs) {
        for (auto varDec : *_varDecs) {
            children.push_back(varDec);
        }
    }
    return children;
}

std::vector<Node*> ExpStmt::getChildren() {
    std::vector<Node*> children;
    if (_exp) {
        children.push_back(_exp);
    }
    return children;
}

std::vector<Node*> IfStmt::getChildren() {
    std::vector<Node*> children;
    if (_cond) {
        children.push_back(_cond);
    }
    if (_then) {
        children.push_back(_then);
    }
    if (_else) {
        children.push_back(_else);
    }
    return children;
}

std::vector<Node*> RetStmt::getChildren() {
    std::vector<Node*> children;
    if (_retval) {
        children.push_back(_retval);
    }
    return children;
}

std::vector<Node*> WhileStmt::getChildren() {
    std::vector<Node*> children;
    if (_cond) {
        children.push_back(_cond);
    }
    if (_body) {
        children.push_back(_body);
    }
    return children;
}

std::vector<Node*> AssignOpExpr::getChildren() {
    std::vector<Node*> children;
    children.push_back(LHS);
    children.push_back(RHS);
    return children;
}

std::vector<Node*> BinaryOpExpr::getChildren() {
    std::vector<Node*> children;
    children.push_back(LHS);
    children.push_back(RHS);
    return children;
}

std::vector<Node*> MinusExpr::getChildren() {
    std::vector<Node*> children;
    children.push_back(Expr);
    return children;
}

std::vector<Node*> NotExpr::getChildren() {
    std::vector<Node*> children;
    children.push_back(Expr);
    return children;
}

std::vector<Node*> ArrayVisitExpr::getChildren() {
    std::vector<Node*> children;
    children.push_back(Array);
    children.push_back(Index);
    return children;
}

std::vector<Node*> IntExpr::getChildren() {
    return std::vector<Node*>();
}

std::vector<Node*> FloatExpr::getChildren() {
    return std::vector<Node*>();
}

std::vector<Node*> IDExpr::getChildren() {
    return std::vector<Node*>();
}

std::vector<Node*> CallFuncExpr::getChildren() {
    std::vector<Node*> children;
    if (_args) {
        for (auto& arg : *_args) {
            children.push_back(arg);
        }
    }
    return children;
}

//generate dot grammar for each node recursively
void generateDot(AST::Node* node, std::ostream& dotFile) {
    std::string nodeID = std::to_string((long)node);
    std::string nodeName = typeid(*node).name();
    nodeName = std::regex_replace(nodeName, std::regex("N\\d+AST\\d+"), "");
    dotFile << nodeID << " [label=\"" << nodeName << "\"];\n";
    for (auto& child : node->getChildren()) {
        std::string childID = std::to_string((long)child);
        std::string childName = typeid(*child).name();
        childName = std::regex_replace(childName, std::regex("N\\d+AST\\d+"), "");
        dotFile << nodeID << " -> " << childID << ";\n";
        generateDot(child, dotFile);
    }
}

//generate dot file for AST
void generateDotFile(AST::Program* program, std::string& dotFilename) {
    std::ofstream dotFile(dotFilename);
    if (!dotFile.is_open()) {
        std::cout << "Failed to open DOT file: " << dotFilename << std::endl;
        return;
    }

    dotFile << "digraph AST {\n";
    generateDot(program, dotFile);
    dotFile << "}\n";

    dotFile.close();
}