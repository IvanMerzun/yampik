#pragma once
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

class Node 
{
private:
    string data;
    vector<Node> children;

public:
    Node(string s) : data(s) {}

    Node& addSon(const string& s) {
        children.emplace_back(s); // ƒобавл€ем узел в список
        return children.back();   // ¬озвращаем ссылку на последний добавленный узел
    }

    Node& getSon(int i) 
    {
        return children.at(i); // Ѕезопасный доступ к дочернему узлу
    }

    int getChildCount() const { // ћетод дл€ получени€ числа дочерних узлов
        return children.size();
    }

    void print(int lvl) const 
    {
        for (int i = 0; i < lvl; ++i) 
        {
            cout << "  ";
        }
        cout << data << endl;
        for (const auto& child : children) 
        {
            child.print(lvl + 1);
        }
    }
};

