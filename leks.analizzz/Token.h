#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <cctype>

enum TokenType {
    INT_NUM, FLOAT_NUM, ID_NAME, OPERATOR, DELIMITER, TYPE, BEGIN, END, ERROR, UNKNOWN, CONST, FUNC_NAME, END_OF_FILE
};

struct Token {
    TokenType type;
    std::string value;

    Token(TokenType t, std::string v) : type(t), value(v) {}
    Token() : type(UNKNOWN), value("") {}
};



