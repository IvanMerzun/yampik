#pragma once
#include "Token.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <algorithm>

using namespace std;

// Лексический анализатор с конечным автоматом
class Lexer {
private:
    std::ifstream inputFile;
    char currentChar;
    bool endOfFile;
    int currentLine; // Текущий номер строки

    void nextChar() 
    {
        if (!inputFile.get(currentChar)) 
        {
            endOfFile = true;
        }
        //std::cout << "Read char: " << currentChar << " at line " << currentLine << std::endl; // Для отладки

        if (currentChar == '\n') {
            currentLine++; // Увеличиваем номер строки
            
        }
    }

    void skipWhitespace() 
    {
        while (std::isspace(currentChar)) 
        {
            nextChar();
        }
    }

    bool isLetter(char c) 
    {
        return std::isalpha(c); // Проверка на буквы (включая заглавные и строчные)
    }

    bool isDigit(char c) 
    {
        return std::isdigit(c);
    }

    Token collectWord() 
    {
        string lexeme;
        // Сначала проверяем операторы
        if (currentChar == '+' || currentChar == '=' || currentChar == '-')
        {
            lexeme += currentChar;
            nextChar(); // Переходим к следующему символу после оператора
            if (currentChar == ';' || currentChar == ')' || currentChar == '(' || currentChar == '{' ||
                currentChar == '}' || currentChar == ' ')
            {
                return Token(OPERATOR, lexeme); // Возвращаем токен оператора
            }
            else
            {
                //собираем ошибочную лексему
                while (!endOfFile && !isspace(currentChar) &&
                    currentChar != ';' && currentChar != ')' &&
                    currentChar != '(' && currentChar != '{' &&
                    currentChar != '}' && currentChar != ',')
                {
                    lexeme += currentChar;
                    nextChar(); // Переходим к следующему символу после оператора
                }
                return Token(ERROR, lexeme);
            }
           
        }

        // Проверяем первый символ
        if (isLetter(currentChar))
        {
            // Если первый символ — буква, начинаем собирать потенциальное ключевое слово или идентификатор
            bool isPotentialKeyword = true;   // Флаг, указывающий на возможность распознавания ключевых слов
            int keywordStep = 0;              // Счётчик шагов для проверки ключевых слов

            // Флаги для распознавания специальных слов
            bool isCheckingInt = true;
            bool isCheckingFloat = true;
            bool isCheckingMain = true;
            bool isCheckingReturn = true;
            bool isCheckingItof = true;
            bool isCheckingFtoi = true;
            //bool isCheckingConst = true;

            // Собирать символы посимвольно
            while (!endOfFile && !isspace(currentChar) &&
                currentChar != ';' && currentChar != ')' &&
                currentChar != '(' && currentChar != '{' &&
                currentChar != '}' && currentChar != ',')
            {
                

                // Пошаговая проверка для "int"
                if (isCheckingInt) 
                {
                    if (keywordStep == 0 && currentChar == 'i') 
                    {
                        keywordStep++;
                    }
                    else if (keywordStep == 1 && currentChar == 'n') 
                    {
                        keywordStep++;
                    }
                    else if (keywordStep == 2 && currentChar == 't') 
                    {
                        keywordStep++;
                        lexeme += currentChar;
                        nextChar();
                        if (currentChar == ' ' || currentChar == ';' || currentChar == '(' || currentChar == ')') 
                        {
                            return Token(TYPE, "int"); // Нашли "int", проверяем на разделитель
                        }
                        else
                        {
                            
                            isCheckingInt = false;
                        }
                    }
                    else 
                    {
                        isCheckingInt = false;
                    }
                }

                // Пошаговая проверка для "float"
                if (isCheckingFloat) 
                {
                    if (keywordStep == 0 && currentChar == 'f') 
                    {
                        keywordStep++;
                    }
                    else if (keywordStep == 1 && currentChar == 'l') 
                    {
                        keywordStep++;
                    }
                    else if (keywordStep == 2 && currentChar == 'o') 
                    {
                        keywordStep++;
                    }
                    else if (keywordStep == 3 && currentChar == 'a') 
                    {
                        keywordStep++;
                    }
                    else if (keywordStep == 4 && currentChar == 't') 
                    {
                        keywordStep++;
                        lexeme += currentChar;
                        nextChar();
                        if (currentChar == ' ' || currentChar == ';' || currentChar == '(' || currentChar == ')') 
                        {
                            return Token(TYPE, "float");
                        }
                        else
                        {
                            isCheckingFloat = false;
                        }
                    }
                    else 
                    {
                        isCheckingFloat = false;
                    }
                }

                // Пошаговая проверка для "main"
                if (isCheckingMain) 
                {
                    if (keywordStep == 0 && currentChar == 'm') 
                    {
                        keywordStep++;
                    }
                    else if (keywordStep == 1 && currentChar == 'a') 
                    {
                        keywordStep++;
                    }
                    else if (keywordStep == 2 && currentChar == 'i') 
                    {
                        keywordStep++;
                    }
                    else if (keywordStep == 3 && currentChar == 'n') 
                    {
                        keywordStep++;
                        lexeme += currentChar;
                        nextChar();
                        if (currentChar == ' ' || currentChar == ';' || currentChar == '(' || currentChar == ')') 
                        {
                            return Token(ID_NAME, "main");
                        }
                        else
                        {
                            isCheckingMain = false;
                        }
                    }
                    else 
                    {
                        isCheckingMain = false;
                    }
                }

                // Пошаговая проверка для "return"
                if (isCheckingReturn) 
                {
                    if (keywordStep == 0 && currentChar == 'r') 
                    {
                        keywordStep++;
                    }
                    else if (keywordStep == 1 && currentChar == 'e') 
                    {
                        keywordStep++;
                    }
                    else if (keywordStep == 2 && currentChar == 't') 
                    {
                        keywordStep++;
                    }
                    else if (keywordStep == 3 && currentChar == 'u') 
                    {
                        keywordStep++;
                    }
                    else if (keywordStep == 4 && currentChar == 'r') 
                    {
                        keywordStep++;
                    }
                    else if (keywordStep == 5 && currentChar == 'n') 
                    {
                        keywordStep++;
                        lexeme += currentChar;
                        nextChar();
                        if (currentChar == ' ' || currentChar == ';' || currentChar == '(' || currentChar == ')') 
                        {
                            return Token(END, "return");
                        }
                        else
                        {
                            isCheckingReturn = false;
                        }
                    }
                    else {
                        isCheckingReturn = false;
                    }
                }

                // Пошаговая проверка для "itof"
                if (isCheckingItof) 
                {
                    if (keywordStep == 0 && currentChar == 'i') 
                    {
                        keywordStep++;
                    }
                    else if (keywordStep == 1 && currentChar == 't') 
                    {
                        keywordStep++;
                    }
                    else if (keywordStep == 2 && currentChar == 'o') 
                    {
                        keywordStep++;
                    }
                    else if (keywordStep == 3 && currentChar == 'f') 
                    {
                        keywordStep++;
                        lexeme += currentChar;
                        nextChar();
                        if (currentChar == ' ' || currentChar == ';' || currentChar == '(' || currentChar == ')') 
                        {
                            return Token(ID_NAME, "itof");
                        }
                        else
                        {
                            isCheckingItof = false;
                        }
                    }
                    else 
                    {
                        isCheckingItof = false;
                    }
                }

                // Пошаговая проверка для "ftoi"
                if (isCheckingFtoi) 
                {
                    if (keywordStep == 0 && currentChar == 'f') 
                    {
                        keywordStep++;
                    }
                    else if (keywordStep == 1 && currentChar == 't') 
                    {
                        keywordStep++;
                    }
                    else if (keywordStep == 2 && currentChar == 'o') 
                    {
                        keywordStep++;
                    }
                    else if (keywordStep == 3 && currentChar == 'i') 
                    {
                        keywordStep++;
                        lexeme += currentChar;
                        nextChar();
                        if (currentChar == ' ' || currentChar == ';' || currentChar == '(' || currentChar == ')') 
                        {
                            return Token(ID_NAME, "ftoi");
                        }
                        else
                        {
                            isCheckingFtoi = false;
                        }
                    }
                    else 
                    {
                        isCheckingFtoi = false;
                    }
                }

                // Если ни одно ключевое слово не распознано, продолжаем сбор лексемы
                if (!isCheckingInt && !isCheckingFloat && !isCheckingMain &&
                    !isCheckingReturn && !isCheckingItof && !isCheckingFtoi)
                {
                    if (!isLetter(currentChar)) 
                    { 
                        //собираем ошибочную лексему
                        while (!endOfFile && !isspace(currentChar) &&
                            currentChar != ';' && currentChar != ')' &&
                            currentChar != '(' && currentChar != '{' &&
                            currentChar != '}' && currentChar != ',')
                        {
                            lexeme += currentChar;
                            nextChar(); // Переходим к следующему символу после оператора
                        }
                        return Token(ERROR, lexeme);
                    }
                }

                lexeme += currentChar;
                nextChar();
            }

            // Раз дошли сюда, значит лексема состоит только из букв и это идентификатор
            return Token(ID_NAME, lexeme);
        }


        else if (isDigit(currentChar))
        {
            bool ffloat = false;
            int k = 0;
            while (!endOfFile && !isspace(currentChar) &&
                currentChar != ';' && currentChar != ')' &&
                currentChar != '(' && currentChar != '{' &&
                currentChar != '}' && currentChar != ',')
            {
                if (!isDigit(currentChar))
                {
                    if (currentChar == '.' && k == 0)
                    {
                        ffloat = true;
                        ++k;
                    }
                    else
                    {
                        while (!endOfFile && !isspace(currentChar) &&
                            currentChar != ';' && currentChar != ')' &&
                            currentChar != '(' && currentChar != '{' &&
                            currentChar != '}' && currentChar != ',')
                        {

                            lexeme += currentChar;
                            nextChar();
                        }
                        return Token(ERROR, lexeme);
                    }
                }

                lexeme += currentChar;
                nextChar();
            }

            if (!ffloat)
            {
                return Token(CONST, lexeme);
            }
            else
            {
                return Token(CONST, lexeme);
            }
        }

        else
        {
           
            
            //собираем ошибочную лексему
            while (!endOfFile && !isspace(currentChar) &&
                currentChar != ';' && currentChar != ')' &&
                currentChar != '(' && currentChar != '{' &&
                currentChar != '}' && currentChar != ',')
            {

                lexeme += currentChar;
                nextChar(); // Переходим к следующему символу после оператора
            }
            return Token(ERROR, lexeme);
        }

       
    }

    
    

public:
    Lexer(const std::string& fileName) : endOfFile(false), currentLine(1)
    {
        inputFile.open(fileName);
        //inputFile << " $";
        nextChar();  // Считываем первый символ
    }

    int getLineNumber() const 
    {
        return currentLine;
    }

    Token getNextToken() 
    {
        skipWhitespace();

        if (endOfFile) 
        {
            return Token(END_OF_FILE, "$"); // Символ $ вместо UNKNOWN
        }

        if (currentChar == ';' || currentChar == ')' || currentChar == '(' || currentChar == '{' ||
            currentChar == '}' || currentChar == ',')
        {
            string lexeme(1, currentChar);
            nextChar();
            return Token(DELIMITER, (lexeme));
            
        }
        

        else
        {
            return collectWord();
        }

        
    }

    ~Lexer() {
        if (inputFile.is_open()) 
        {
            inputFile.close();
        }
    }
};






