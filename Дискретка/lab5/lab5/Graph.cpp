#include "Graph.h"
#include <fstream>
#include <iostream>
#include <chrono>

Graph::HungarianAlgorithm::HungarianAlgorithm(std::string filename) {
    std::cout << "Start\n";
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Cant open file\n";
        return;
    }

    file >> this->n;
    cost.resize(this->n, std::vector<int>(this->n));
    
    for (int i = 0; i < this->n; i++)
        for (int j = 0; j < this->n; j++)
            file >> cost[i][j];

    Xa.resize(n + 1);
    Xb.resize(n + 1);
    p.resize(n + 1);
    way.resize(n + 1);
    std::cout << "End\n";
}

int Graph::HungarianAlgorithm::solve() {
    // Ѕежим по Xa - левой доли графа
    for (int i = 1; i <= n; i++) {
        // p[j] - смежна€ вершина левой доли с правой
        // начнем с нулевой
        p[0] = i;
        int j0 = 0;
        // minv - минимальный стоимости
        std::vector<int> minv(n + 1, std::numeric_limits<int>::max());
        // used - есть ли вершина в дереве
        std::vector<bool> used(n + 1, false);

        do {
            used[j0] = true;
            int i0 = p[j0];
            // минимальное из minv
            int delta = std::numeric_limits<int>::max();
            int j1 = 0;

            for (int j = 1; j <= n; j++) {
                // ƒл€ ребер не в дереве
                // находим минимальную delta
                if (!used[j]) {
                    int cur = cost[i0 - 1][j - 1] - Xa[i0] - Xb[j];
                    if (cur < minv[j]) {
                        minv[j] = cur;
                        way[j] = j0;
                    }
                    if (minv[j] < delta) {
                        delta = minv[j];
                        j1 = j;
                    }
                }
            }
            // ѕересчет долей
            for (int j = 0; j <= n; j++) {
                if (used[j]) {
                    Xa[p[j]] += delta;
                    Xb[j] -= delta;
                }
                else {
                    minv[j] -= delta;
                }
            }

            j0 = j1;
        } while (p[j0] != 0);

        // југментальное дерево
        do {
            int j1 = way[j0];
            p[j0] = p[j1];
            j0 = j1;
        } while (j0);
    }

    return -Xb[0]; // минимальна€ стоимость
}

std::vector<std::pair<int, int>> Graph::HungarianAlgorithm::getMatching() {
    std::vector<std::pair<int, int>> matching;
    for (int j = 1; j <= n; j++) {
        if (p[j] != 0) {
            matching.push_back({ p[j] - 1, j - 1 });
        }
    }
    return matching;
}

void Graph::HungarianAlgorithm::write_result() {
    auto start = std::chrono::high_resolution_clock::now();

    auto weight = this->solve();
    auto res = this->getMatching();

    auto end = std::chrono::high_resolution_clock::now();
    auto working_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << weight << "\n";

    auto total_ms = working_time.count();
    long long minutes = total_ms / 60000;
    long long seconds = (total_ms % 60000) / 1000;
    long long milliseconds = total_ms % 1000;

    std::ofstream output("Hungarian.txt");
    if (!output.is_open())
        return;

    std::cout << "Time working: " << minutes << "m " << seconds << "s " << milliseconds << "ms\n";
    output << "Time working: " << minutes << "m " << seconds << "s " << milliseconds << "ms\n";

    for (const auto& [u, v] : res) {
        output << "{ " << u << ", " << v << " }\n";
    }
    output.close();
    return;
}
