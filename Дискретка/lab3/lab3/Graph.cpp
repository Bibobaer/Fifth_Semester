#include "Graph.h"

#include <fstream>
#include <iostream>
#include <numeric>
#include <chrono>
#include <algorithm>

Graph::ListGraph::ListGraph(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Cant open file");

    file >> this->n;

    graph.resize(this->n);

    for (int i = 0; i < this->n; i++) {
        for (int j = 0; j < this->n; j++) {
            int value;
            file >> value;
            if (value == 1 && i < j) {
                graph[i].push_back(j);
                graph[j].push_back(i);
            }
        }
    }
}

bool Graph::ListGraph::AreAdjacent(int u, int v) {
    if (u == v) return true;

    for (auto& nei : graph[u])
        if (nei == v)
            return true;
    return false;
}

void Graph::ListGraph::extend(std::set<int>& S, std::set<int> Q_plus, std::set<int> Q_minus) {
    // Пока Q+ не пусто
    while (!Q_plus.empty()) {
        // Существует ли вершина в Q-, не соединенная ни с одной вершиной из Q+
        bool isolated_in_q_minus = false;
        for (auto& u : Q_minus) {
            bool connected = false;
            for (auto& v : Q_plus) {
                if (AreAdjacent(u, v)) {
                    connected = true;
                    break;
                }
            }
            if (!connected) {
                isolated_in_q_minus = true;
                break;
            }
        }

        if (isolated_in_q_minus) {
            break;
        }

        // Выбираем вершину v из Q+
        // И добавляем в S
        auto v = *Q_plus.begin();
        S.insert(v);

        std::set<int> newQplus, newQminus;

        // Удаляем из Q+ и Q- вершины, соединенные с v
        for (int u : Q_plus) {
            if (!AreAdjacent(v, u)) {
                newQplus.insert(u);
            }
        }

        for (int u : Q_minus) {
            if (!AreAdjacent(v, u)) {
                newQminus.insert(u);
            }
        }

        // Если newQ+ и newQ- пусты, то S - максимальное независимое множество
        if (!newQplus.size() && !newQminus.size()) {
            maximalIndependentSets.push_back(S);
            if (S.size() > maxIndependentSetSize) {
                maxIndependentSetSize = S.size();
            }
        }
        else {
            extend(S, newQplus, newQminus);
        }

        // Шаг возврата
        S.erase(v);
        Q_plus.erase(v);
        Q_minus.insert(v);
    }
}

void Graph::ListGraph::BroneKerbosh() {
    maximalIndependentSets.clear();
    maxIndependentSetSize = 0;

    std::set<int> S;
    std::set<int> initialQplus;
    std::set<int> initialQminus;

    for (int i = 0; i < this->n; i++) {
        initialQplus.insert(i);
    }

    auto start = std::chrono::high_resolution_clock::now();
    extend(S, initialQplus, initialQminus);
    auto end = std::chrono::high_resolution_clock::now();
    auto working_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    auto total_ms = working_time.count();
    long long minutes = total_ms / 60000;
    long long seconds = (total_ms % 60000) / 1000;
    long long milliseconds = total_ms % 1000;

    std::cout << "Count Independents of graph: " << maxIndependentSetSize << "\n";
    std::cout << "Count of max Independents set: " << maximalIndependentSets.size() << "\n";

    std::ofstream out("BroneKerbosh.txt");
    if (!out.is_open()) {
        std::cout << "Error\n";
        return;
    }
    out << "Time working: " << minutes << "m " << seconds << "s " << milliseconds << "ms\n";

    for (const auto& independentSet : maximalIndependentSets) {
        for (int vertex : independentSet) {
            out << vertex << " ";
        }
        out << std::endl;
    }
    out.close();
}

void Graph::ListGraph::Print() {
    for (int i = 0; i < graph.size(); i++) {
        std::cout << "Vertex " << i << ": ";
        for (int j = 0; j < graph[i].size(); j++) {
            std::cout << graph[i][j] << " ";
        }
        std::cout << "\n";
    }
}