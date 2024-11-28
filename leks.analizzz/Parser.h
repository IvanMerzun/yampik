#pragma once
#include "Lexer.h"
#include "Node.h"
#include "HashT.h"

class Parser 
{
private:
    Lexer lexer;
    HashT symbolTable;
    Token currentToken;

    void nextToken();

    // Методы для грамматики
    void Function(Node& n);
    void Begin(Node& n);
    void End(Node& n);
    void Descriptions(Node& n);
    void Operators(Node& n);
    void Descr(Node& n);
    void VarList(Node& n);
    void Type(Node& n);
    void Op(Node& n);
    void SimpleExpr(Node& n);
    void Expr(Node& n);
    void Id(Node& n);
    void Const(Node& n);
    void FunctionName(Node& n);

public:
    Parser(const std::string& inputFile);
    Node parse();
};

