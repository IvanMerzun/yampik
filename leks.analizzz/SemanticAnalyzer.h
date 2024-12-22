#pragma once
#include <fstream>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include "Node.h"

class SemanticAnalyzer
{
private:
    std::ofstream outFile; // Файл для записи
    string typemain, idret; 
    struct VariableInfo {
        std::string type;  // Тип переменной
    };

    // Таблица символов (множество всех объявленных переменных)
    std::unordered_map<std::string, VariableInfo> declaredVariables;

    // Рекурсивная обработка дерева
    void processNode(const Node& node) {

        if (node.getData() == "Begin") {
            processBegin(node);
        }

        if (node.getData() == "Descriptions") {
            processDescriptions(node);
        }

        if (node.getData() == "Operators") {
            processOperators(node); // Новый обработчик для операторов
        }

        if (node.getData() == "End") {
            processEnd(node);
        }

        // Рекурсивно обрабатываем все дочерние узлы
        for (int i = 0; i < node.getChildCount(); ++i) {
            processNode(node.getSon(i));
        }
    }

    void processBegin(const Node& node)
    {
        typemain = node.getSon(0).getSon(0).getData();
    }

    void processEnd(const Node& node)
    {
        idret = node.getSon(1).getSon(0).getData();

        // Проверяем, существует ли эта переменная в таблице символов
        if (declaredVariables.find(idret) == declaredVariables.end()) {
            throw std::runtime_error("Variable '" + idret + "' used before declaration.");
        }

        // Проверяем, что тип возвращаемой переменной совпадает с типом функции
        if (declaredVariables[idret].type != typemain) {
            throw std::runtime_error("Expected in return " + typemain + ", but got " + declaredVariables[idret].type + ".");
        }
    }


    // Обработка Descriptions
    void processDescriptions(const Node& node) {
        for (int i = 0; i < node.getChildCount(); ++i) {
            if (node.getSon(i).getData() == "Descr") {
                processDescr(node.getSon(i));
            }
        }
    }

    // Обработка Descr
    void processDescr(const Node& node) {
        std::string type = node.getSon(0).getSon(0).getData(); // Тип переменных (int, float)

        std::vector<std::string> variables;
        if (node.getSon(1).getData() == "VarList")
        {
            collectVariables(node.getSon(1), variables);
        }

        // Проверяем, что переменные еще не были объявлены
        for (const auto& var : variables) {
            if (declaredVariables.find(var) != declaredVariables.end()) {
                // Ошибка: переменная уже объявлена
                throw std::runtime_error("Variable '" + var + "' is already declared.");
            }
            else {
                // Добавляем переменную в таблицу символов с её типом
                declaredVariables[var] = { type };
            }
        }

        // Формируем строку для постфиксной записи
        std::string result = type; // Начинаем с типа данных
        if (variables.size() == 1)
        {
            for (const auto& var : variables) {
                result += " " + var; // Добавляем имена переменных
            }
            result += " DECL"; // Добавляем количество и DECL
        }
        else
        {
            for (const auto& var : variables)
            {
                result += " " + var; // Добавляем имена переменных
            }
            result += " " + std::to_string(variables.size() + 1) + " DECL"; // Добавляем количество и DECL
        }

        // Записываем строку в файл
        outFile << result << std::endl;
    }

    // Рекурсивный сбор переменных из VarList
    void collectVariables(const Node& node, std::vector<std::string>& variables)
    {
        for (int i = 0; i < node.getChildCount(); ++i)
        {
            if (node.getSon(i).getData() == "Id")
            {
                variables.push_back(node.getSon(i).getSon(0).getData());
            }
        }
    }

    void processOperators(const Node& node)
    {
        for (int i = 0; i < node.getChildCount(); ++i)
        {
            if (node.getSon(i).getData() == "Op")
            {
                processOp(node.getSon(i));
            }
        }
    }

    void processOp(const Node& node)
    {
        std::string varName = node.getSon(0).getSon(0).getData();  // Получаем имя переменной (Id)

        // Извлекаем тип выражения справа от оператора
        std::string exprType = getExprType(node.getSon(2)); //передаем Expr

        // Проверяем тип переменной слева и справа от оператора
        if (declaredVariables.find(varName) == declaredVariables.end()) {
            // Ошибка: переменная используется до объявления
            throw std::runtime_error("Variable '" + varName + "' used before declaration.");
        }

        std::string varType = declaredVariables[varName].type;

        // Проверяем, что тип переменной и выражения совпадают
        if (varType != exprType) {
            throw std::runtime_error("Type mismatch: variable '" + varName + "' is of type '" + varType +
                "' but expression is of type '" + exprType + "'.");
        }

        string exprPostfix = processExpr(node.getSon(2));  // Получаем постфиксную запись для выражения
        string result = varName + " " + exprPostfix + " =";

        outFile << result << std::endl;

       
    }

    string getExprType(const Node& node) 
    {
        
        if ( node.getChildCount() == 1)
        {
            if (node.getSon(0).getSon(0).getData() == "Const")
            {
                std::string value = node.getSon(0).getSon(0).getSon(0).getData();

                // Если в значении есть точка, считаем это float, иначе int
                if (value.find('.') != std::string::npos)
                {
                    return "float";  // Число с плавающей точкой
                }
                else {
                    return "int";  // Целое число
                }
            }

            // Если это переменная, то возвращаем её тип из таблицы символов
            if (node.getSon(0).getSon(0).getData() == "Id") {
                std::string varName = node.getSon(0).getSon(0).getSon(0).getData();
                if (declaredVariables.find(varName) != declaredVariables.end()) {
                    return declaredVariables[varName].type;
                }
                else {
                    throw std::runtime_error("Variable '" + varName + "' used before declaration.");
                }
            }

            if (node.getSon(0).getSon(0).getData() == "itof")
            {
                string rezu = getExprType(node.getSon(0).getSon(2));
                if (rezu == "int")
                {
                    return "float";
                }
                else
                {
                    throw std::runtime_error("Expected int in itof.");
                }
            }

            if (node.getSon(0).getSon(0).getData() == "ftoi")
            {
                string rezu = getExprType(node.getSon(0).getSon(2));
                if (rezu == "float")
                {
                    return "int";
                }
                else
                {
                    throw std::runtime_error("Expected float in ftoi.");
                }
            }

        }
        //берем тип первого первого при помощи getExprTypeNeed, далее проверяем циклом, чтобы остальные операнды справа были точно такие же, находим хоть один не такой, значит ошибка. 

        else if (node.getChildCount() > 1)
        {

            string currentType = getExprTypeNeed(node.getSon(0)); // Инициализируем тип первого операнда

            for (int i = 1; i < node.getChildCount(); ++i)
            {
                // Пропускаем операторы
                if (node.getSon(i).getData() == "+" || node.getSon(i).getData() == "-") {
                    continue;
                }

                // Обрабатываем SimpleExpr
                if (node.getSon(i).getData() == "SimpleExpr") {
                    string operandType = getExprTypeNeed(node.getSon(i));  // Вызов функции для получения типа

                    // Проверяем совместимость типов
                    if (currentType != operandType) {
                        throw std::runtime_error("Type mismatch in arithmetic expression.");
                    }
                }
            }

            // Возвращаем итоговый тип выражения
            return currentType;
        }
        
        
    }

    string getExprTypeNeed(const Node& node)
    {
        if (node.getSon(0).getData() == "Const")
        {
            std::string value = node.getSon(0).getSon(0).getData();

            // Если в значении есть точка, считаем это float, иначе int
            if (value.find('.') != std::string::npos)
            {
                return "float";  // Число с плавающей точкой
            }
            else {
                return "int";  // Целое число
            }
        }

        // Если это переменная, то возвращаем её тип из таблицы символов
        if (node.getSon(0).getData() == "Id") {
            std::string varName = node.getSon(0).getSon(0).getData();
            if (declaredVariables.find(varName) != declaredVariables.end()) {
                return declaredVariables[varName].type;
            }
            else {
                throw std::runtime_error("Variable '" + varName + "' used before declaration.");
            }
        }

        if (node.getSon(0).getData() == "itof")
        {
            string rezu = getExprType(node.getSon(2));
            if (rezu == "int")
            {
                return "float";
            }
            else
            {
                throw std::runtime_error("Expected int in itof.");
            }
        }

        if (node.getSon(0).getData() == "ftoi")
        {
            string rezu = getExprType(node.getSon(2));
            if (rezu == "float")
            {
                return "int";
            }
            else
            {
                throw std::runtime_error("Expected float in ftoi.");
            }
        }
    }


    string processExpr(const Node& node)
    {
        string rez;

        for (int i = 0; i < node.getChildCount(); ++i)
        {
            if (node.getSon(i).getData() == "SimpleExpr")
            {
                rez += " " + processSimpleExpr(node.getSon(i));  // Обрабатываем операнды
            }
        }

        for (int i = 0; i < node.getChildCount(); ++i)
        {
            if (node.getSon(i).getData() == "+" || node.getSon(i).getData() == "-")
            {
                rez += " " + node.getSon(i).getData();  // Добавляем оператор
            }
        }

        return rez;
    }

    string processSimpleExpr(const Node& node)
    {
        string result;
        if (node.getSon(0).getData() == "Const")
        {
            result += node.getSon(0).getSon(0).getData();  // Константа
        }
        else if (node.getSon(0).getData() == "Id")
        {
            result += node.getSon(0).getSon(0).getData();  // Идентификатор
        }
        else if (node.getSon(0).getData() == "ftoi" || node.getSon(0).getData() == "itof")
        {
            result += processFunctionCall(node) + "CALL";  // Обрабатываем функцию
        }

        return result;
    }

    string processFunctionCall(const Node& node)
    {
        string result = node.getSon(0).getData();

        result += " ";
        result += processExpr(node.getSon(2));  // Обрабатываем выражение внутри скобок
        result += " ";

        return result;
    }



public:
    SemanticAnalyzer(const std::string& outputFileName) {
        outFile.open(outputFileName);
        if (!outFile.is_open()) {
            throw std::runtime_error("Failed to open output file for semantic analysis.");
        }
    }

    ~SemanticAnalyzer() {
        if (outFile.is_open()) {
            outFile.close();
        }
    }

    void analyze(const Node& root) {
        try {
            processNode(root);

            // Если ошибок нет, записываем постфиксную запись
            outFile << "Postfix notation successfully generated." << std::endl;
        }
        catch (const std::runtime_error& e) {
            // Если ошибка возникла, выводим ее в файл
            outFile << "Error: " << e.what() << std::endl;
        }
    }
};