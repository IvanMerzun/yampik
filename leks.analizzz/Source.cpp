#include "Token.h"
#include "Parser.h"
#include "HashT.h"
#include "Lexer.h"
#include "SemanticAnalyzer.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cctype>

using namespace std;


int main()
{


    //// Открываем входной файл
    //std::ifstream inputFile("input.txt");
    //if (!inputFile.is_open()) {
    //    std::cerr << "Error: Unable to open input file!" << std::endl;
    //    return 1;
    //}

    //// Создаем лексический анализатор
    //Lexer lexer("input.txt");

    //// Создаем хеш-таблицу
    //HashT hashTable;

    //// Считываем токены и вставляем их в хеш-таблицу
    //Token token = lexer.getNextToken();
    //while (token.type != UNKNOWN) {
    //    hashTable.insert(token);
    //    token = lexer.getNextToken();
    //}

    //std::ofstream outputFile("output.txt");
    //if (!outputFile.is_open()) {
    //    std::cerr << "Error: Unable to open output file!" << std::endl;
    //    return 1;
    //}

    //// Вывод содержимого хеш-таблицы в файл
    //hashTable.print(outputFile);

    //return 0;
    try
    {
        Parser parser("input.txt");
        Node ast = parser.parse();
        ofstream outFile("outparser.txt");
        ast.print(outFile);
        outFile.close();


        SemanticAnalyzer semanticAnalyzer("outsemat.txt");
        semanticAnalyzer.analyze(ast);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Parsing error: " << e.what() << std::endl;
    }
    return 0;
}