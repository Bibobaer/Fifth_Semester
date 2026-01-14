#pragma once
#include "main.hpp"
#include <stack>
#include <fstream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <random>
#include <string>

using namespace std;
using namespace std::chrono;

enum DataType {
    RANDOM,
    SORTED,
    SEMI_SORTED,
    UNIQUE,
    DUPLICATES
};

static ofstream file;

vector<int> generateVector(DataType type, int size) {
    vector<int> out;
    out.reserve(size);

    random_device rd;
    mt19937 gen(rd());

    switch (type) {
    case RANDOM: {
        uniform_int_distribution<int> dist(1, size);
        for (int i = 0; i < size; i++)
            out.push_back(dist(gen));
        break;
    }
    case SORTED:
        for (int i = 1; i <= size; i++)
            out.push_back(i);
        break;
    case SEMI_SORTED:
        for (int i = 1; i <= size; i++)
            out.push_back(i);
        for (int j = 0; j < size / 10; j++) {
            int ind1 = gen() % size;
            int ind2 = gen() % size;
            std::swap(out[ind1], out[ind2]);
        }
        break;
    case UNIQUE:
        for (int i = 1; i <= size; i++)
            out.push_back(i);
        shuffle(out.begin(), out.end(), gen);
        break;
    case DUPLICATES: {
        uniform_int_distribution<int> dist(1, size / 10);
        for (int i = 0; i < size; i++)
            out.push_back(dist(gen));
        break;
    }
    default:
        break;
    }
    return out;
}

template <typename StackType, typename SortFunc>
long long testSort(vector<int>& data, SortFunc func, const string& stackName, const string& sortName, const string& dataType) {
    StackType s;
    for (auto& el : data) {
        s.push(el);
    }

    auto start = high_resolution_clock::now();
    func(s);
    auto end = high_resolution_clock::now();

    auto duration = duration_cast<milliseconds>(end - start).count();

    file << stackName << " | " << sortName << " | " << dataType
        << " | " << data.size() << " | " << duration << " ms" << endl;

    return duration;
}

void runPerformanceTests() {
    const int TEST_SIZES[] = { 1000000 };
    const DataType DATA_TYPES[] = { RANDOM, SORTED, SEMI_SORTED, UNIQUE, DUPLICATES };
    file.open("database.log");
    if (!file.is_open()) return;

    vector<string> dataTypeNames = { "Random     ", "Sorted     ", "Semi Sorted", "Unique     ", "Duplicates " };
    file << "Stack Type | Sort Type |  Data Type  |  Size   | Time (ms)" << endl;
    file << "-----------|-----------|-------------|---------|-----------" << endl;

    for (int size : TEST_SIZES) {
        for (int i = 0; i < 5; ++i) {
            DataType dataType = DATA_TYPES[i];
            auto testData = generateVector(dataType, size);

            // ArrayStack
            testSort<ArrayStack<int>>(testData, mergeSort<ArrayStack<int>>,
                "ArrayStack", "Selection", dataTypeNames[i]);
            testSort<ArrayStack<int>>(testData, quick_sort<ArrayStack<int>>,
                "ArrayStack", "Quick    ", dataTypeNames[i]);

            // ListStack
            testSort<ListStack<int>>(testData, mergeSort<ListStack<int>>,
                "ListStack ", "Selection", dataTypeNames[i]);
            testSort<ListStack<int>>(testData, quick_sort<ListStack<int>>,
                "ListStack ", "Quick    ", dataTypeNames[i]);

            // ChunkStack
            testSort<ChunkStack<int>>(testData, mergeSort<ChunkStack<int>>,
                "ChunkStack", "Selection", dataTypeNames[i]);
            testSort<ChunkStack<int>>(testData, quick_sort<ChunkStack<int>>,
                "ChunkStack", "Quick    ", dataTypeNames[i]);

            // std::stack
            testSort<stack<int>>(testData, mergeSort<stack<int>>,
                "std::stack", "Selection", dataTypeNames[i]);
            testSort<stack<int>>(testData, quick_sort<stack<int>>,
                "std::stack", "Quick    ", dataTypeNames[i]);
        }
    }
    file.close();
}