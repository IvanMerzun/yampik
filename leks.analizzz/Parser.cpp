#include "Parser.h"
#include <stdexcept>
using namespace std;

Parser::Parser(const std::string& inputFile) : lexer(inputFile), currentToken(Token(UNKNOWN, "")) {}
            
void Parser::nextToken() {
    currentToken = lexer.getNextToken();
    
    //cout << "Next token: " << currentToken.value << " at line " << lexer.getLineNumber() << std::endl; // Отладка

    if (currentToken.type != END_OF_FILE) {
        symbolTable.insert(currentToken);
    }
    if (currentToken.type == ERROR)
    {
        throw runtime_error("Error lexem at line " + to_string(lexer.getLineNumber()));
    }
}

Node Parser::parse() {
    Node root("Function");
    nextToken();
    Function(root);
    if (currentToken.type != END_OF_FILE) {
        throw std::runtime_error("Extra tokens after end of parsing at line " + to_string(lexer.getLineNumber()));
    }
    return root;
}

// Правила грамматики
void Parser::Function(Node& n) 
{
    n.addSon("Begin");
    Begin(n.getSon(0)); // Добавляем узел Begin

    n.addSon("Descriptions");
    Descriptions(n.getSon(1)); // Узел для Descriptions

    n.addSon("Operators");
    Operators(n.getSon(2)); // Узел для Operators

    n.addSon("End");
    End(n.getSon(3)); // Узел для End
} 
void Parser::Begin(Node& n) 
{  
    
    if (currentToken.type == TYPE) {
        n.addSon("Type");
        Type(n.getSon(0)); // Узел Type

        n.addSon("FunctionName");
        FunctionName(n.getSon(1)); // Узел FunctionName

        if (currentToken.value == "(") {
            n.addSon("(");
            nextToken();
            if (currentToken.value == ")") {
                n.addSon(")");
                nextToken();
                if (currentToken.value == "{") {
                    n.addSon("{");
                    nextToken();
                }
                else {
                    throw std::runtime_error("Expected '{' after function declaration at line " + to_string(lexer.getLineNumber()));
                }
            }
            else {
                throw std::runtime_error("Expected ')' after function name at line " + to_string(lexer.getLineNumber()-2));
            }
        }
        else {
            throw std::runtime_error("Expected '(' after function name at line " + to_string(lexer.getLineNumber()-1));
        }
    }
    else {
        throw std::runtime_error("Expected type for function at line " + to_string(lexer.getLineNumber()));
    }
}
void Parser::End(Node& n) {
    
    if (currentToken.value == "return") {
        n.addSon("return");
        nextToken();

        n.addSon("Id");
        Id(n.getSon(1)); // Узел для Id

        if (currentToken.value == ";") {
            n.addSon(";");
            nextToken();

            if (currentToken.value == "}") {
                n.addSon("}");
                nextToken();
            }
            else {
                throw std::runtime_error("Expected '}' at the end of the function at line " + to_string(lexer.getLineNumber()));
            }
        }
        else {
            throw std::runtime_error("Expected ';' after return statement at line " + to_string(lexer.getLineNumber()-1));
        }
    }
    else {
        throw std::runtime_error("Expected 'return' statement in function end at line " + to_string(lexer.getLineNumber()));
    }
}
void Parser::Descriptions(Node& n) {
    if (currentToken.type == TYPE) {
        // Добавляем Descr только для одного описания переменной
        n.addSon("Descr");
        Descr(n.getSon(n.getChildCount() - 1)); // Узел Descr для текущего описания переменной

        // Рекурсивно вызываем Descriptions для обработки следующего описания переменной
        Descriptions(n);
    }
    
}
void Parser::Operators(Node& n) {
    if (currentToken.type == ID_NAME) {
        n.addSon("Op");
        Op(n.getSon(n.getChildCount() - 1)); // Узел Op

        Operators(n);
    }
    
}
void Parser::Descr(Node& n) {
    n.addSon("Type");
    Type(n.getSon(0)); // Узел для Type

    n.addSon("VarList");
    VarList(n.getSon(1)); // Узел для VarList

    if (currentToken.value == ";") {
        n.addSon(";");
        nextToken();
    }
    else {
        throw std::runtime_error("Expected ';' after variable description at line " + to_string(lexer.getLineNumber()-1));
    }
}
void Parser::VarList(Node& n) {
   
    // Начнем с добавления первого идентификатора в VarList
    n.addSon("Id");
    Id(n.getSon(0)); // Узел для первого Id

    // Теперь обрабатываем запятые и следующие идентификаторы
    while (currentToken.value == ",") {
        n.addSon(",");
        nextToken(); // Пропускаем запятую

        // Добавляем следующий Id
        n.addSon("Id");
        Id(n.getSon(n.getChildCount() - 1)); // Получаем последний добавленный узел и добавляем новый Id
    }
}
void Parser::Type(Node& n) {
    if (currentToken.type == TYPE) {
        n.addSon(currentToken.value); // Узел для типа (int или float)
        nextToken();
    }
    else {
        throw std::runtime_error("Expected type specifier (int or float) at line " + to_string(lexer.getLineNumber()));
    }
}
void Parser::Op(Node& n) {
    n.addSon("Id");
    Id(n.getSon(0)); // Узел Id

    if (currentToken.value == "=") {
        n.addSon("=");
        nextToken();

        // Создаем узел Expr для всего выражения
        Node& exprNode = n.addSon("Expr");
        Expr(exprNode); // Разбираем выражение в узел Expr

        if (currentToken.value == ";") {
            n.addSon(";");
            nextToken();
        }
        else {
            throw std::runtime_error("Expected ';' after assignment at line " + to_string(lexer.getLineNumber()));
        }
    }
    else {
        throw std::runtime_error("Expected '=' in assignment at line " + to_string(lexer.getLineNumber()));
    }
}
void Parser::SimpleExpr(Node& n) {

    if (currentToken.value == "ftoi" || currentToken.value == "itof") {
        n.addSon(currentToken.value); // Добавляем функцию (ftoi или itof)
        nextToken();

        if (currentToken.value == "(") {
            n.addSon("("); // Добавляем открывающую скобку
            nextToken();

            Node& exprNode = n.addSon("Expr");
            Expr(exprNode); // Разбираем выражение внутри скобок

            if (currentToken.value == ")") {
                n.addSon(")"); // Добавляем закрывающую скобку
                nextToken();
            }
            else {
                throw std::runtime_error("Expected ')' after function argument at line " + to_string(lexer.getLineNumber()));
            }
        }
        else {
            throw std::runtime_error("Expected '(' after function call at line " + to_string(lexer.getLineNumber()));
        }
    }
    else if (currentToken.type == ID_NAME) {
        n.addSon("Id");
        Id(n.getSon(0)); // Узел для идентификатора
    }
    else if (currentToken.type == CONST) {
        n.addSon("Const");
        Const(n.getSon(0)); // Узел для константы
    }
    else if (currentToken.value == "(") {
        n.addSon("(");
        nextToken();

        Node& exprNode = n.addSon("Expr");
        Expr(exprNode); // Вложенное выражение

        if (currentToken.value == ")") {
            n.addSon(")");
            nextToken();
        }
        else {
            throw std::runtime_error("Expected ')' after expression at line " + to_string(lexer.getLineNumber()));
        }
    }
    else {
        throw std::runtime_error("Invalid simple expression at line " + to_string(lexer.getLineNumber()));
    }
}
void Parser::Expr(Node& n) {
    // Разбираем первый операнд
    n.addSon("SimpleExpr");
    SimpleExpr(n.getSon(0));

    // Обрабатываем цепочку операций
    while (currentToken.value == "+" || currentToken.value == "-") {
        n.addSon(currentToken.value); // Узел для оператора
        nextToken();

        n.addSon("SimpleExpr");
        SimpleExpr(n.getSon(n.getChildCount() - 1)); // Используем getChildCount()
    }
}
void Parser::Id(Node& n) {
    if (currentToken.type == ID_NAME) {
        n.addSon(currentToken.value); // Узел для идентификатора
        nextToken();
    }
    else {
        throw std::runtime_error("Expected identifier an line " + to_string(lexer.getLineNumber()));
    }
}
void Parser::Const(Node& n) {
    if (currentToken.type == CONST) {
        n.addSon(currentToken.value); // Узел для константы
        nextToken();
    }
    else {
        throw std::runtime_error("Expected constant at line " + to_string(lexer.getLineNumber()));
    }
}

void Parser::FunctionName(Node& n)
{
    if (currentToken.type == ID_NAME) {
        n.addSon(currentToken.value); // Добавляем идентификатор как дочерний узел
        nextToken();
    }
    else {
        throw std::runtime_error("Expected function name (identifier) at line " + to_string(lexer.getLineNumber()));
    }
}
