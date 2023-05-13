# compiler2023
group work for compiler 2023 spring

## AST
### TODO:
好像文档中的语法没有指针？需要做吗？
以及文档中好像没有位运算等二元操作，好像可扩展性挺强的……
括号表达式有必要吗？……

### 可能要把文法稍微改一下(
1.把Specifier具体分成了VarType等等(感觉声明是`Type ID`而不是`ID`
2.Exp加了个for `FOR LP Exp SEMI Exp SEMI Exp RP Stmt`