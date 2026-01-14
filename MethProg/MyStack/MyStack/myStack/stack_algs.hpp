#pragma once
#include <string>
#include "func.hpp"
#include <stack>
#include <utility>



bool calc_bracket_expression(std::string& str) {
    std::stack<char> S;

    for (auto& sym : str) {
        if (sym == '(' || sym == '[' || sym == '{') {
            S.push(sym);
        }
        else if (sym == ')') {
            if (S.top() != '(')
                return false;
            S.pop();
        }
        else if (sym == ']') {
            if (S.top() != '[')
                return false;
            S.pop();
        }
        else if (sym == '}') {
            if (S.top() != '{')
                return false;
            S.pop();
        }
    }
    return S.size() == 0 ? true : false;
}

void _test_brackets(std::string& str) {
    bool result = calc_bracket_expression(str);

    std::cout << str << std::endl;

    switch (result) {
    case true:
        std::cout << "Good!" << std::endl;
        break;
    case false:
        std::cout << "Das is not shvaine :(\n";
        break;
    }
}

//*Наиболее частые элементы*
//Написать функции, которые в данном наборе элементов находят элемент, который
//- встречается * хотя бы * половину раз от общего числа элементов
//- встречается больше трети раз от общего числа элементов
//- встречается хотя бы 31 % раз от общего числа элементов

template <typename T>
T majoringElement(std::vector<T>& elements) {
    std::stack<T> S;

    for (auto& el : elements) {
        if (S.size() == 0) {
            S.push(el);
        }
        else {
            if (S.top() == el)
                S.push(el);
            else {
                S.pop();
            }
        }
    }
    if (S.size() == 0) {
        throw std::exception("No elements");
    }
    return S.top();
}

template <typename T>
int countMajoring(std::vector<T>& elements) {
    try {
        T maj = majoringElement(elements);
        int count = 0;
        for (auto& el : elements) {
            if (maj == el)
                count++;
        }
        return count;
    }
    catch (...) { }
}

void test_majoring() {
    std::vector<int> nums = { 1, 1, 2, 3 };
    try {
        int maj = majoringElement(nums);
        int count = countMajoring(nums);

        std::cout << "Majoring el: " << maj << " Count: " << count << "\n";
    }
    catch(...) { }
    
}