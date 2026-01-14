#include "Graph.h"

#include <fstream>
#include <iostream>
#include <numeric>
#include <chrono>
#include <stack>
#include <queue>
#include <algorithm>
#include <random>

struct Edge {
    int u, v;
    int weight;

    Edge(int u, int v, int weight) : u(u), v(v), weight(weight) {}

    bool operator<(const Edge& other) const {
        return weight < other.weight;
    }
};

Graph::ListGraph::ListGraph(const std::string& filename) {
    std::cout << "Start Construction\n";
    std::ifstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Cant open file");

    file >> this->n;

    graph.resize(this->n);

    int weight;
    for (int i = 0; i < this->n; i++) {
        for (int j = 0; j < this->n; j++) {
            file >> weight;
            if (weight != 0)
                graph[i].push_back(std::make_pair(j, std::move(weight)));
        }
    }
    std::cout << "End Constructing\n";
}

void Graph::ListGraph::PrintGraph() {
    std::cout << "[ ";
    for (const auto& vertex : graph) {
        std::cout << "[ ";

        for (const auto& connected : vertex) {
            std::cout << "(" << connected.first << ", " << connected.second << ") ";
        }

        std::cout << "], ";
    }
    std::cout << " ]\n";
}

bool Graph::ListGraph::IsConnected() {
    if (this->n == 0) return true;

    std::stack<int> S;
    std::vector<bool> visited(this->n, false);
    int visited_count = 1;

    S.push(0);
    visited[0] = true;

    while (S.size()) {
        int u = S.top();
        S.pop();

        for (auto& ver : graph[u]) {
            if (!visited[ver.first]) {
                visited[ver.first] = true;
                visited_count++;
                S.push(ver.first);
            }
        }
    }
    return visited_count == this->n;
}

void Graph::ListGraph::Kraskal() {
    auto start = std::chrono::high_resolution_clock::now();
    if (!IsConnected()) {
        std::cout << "Graph isnt connected\n";
        return;
    }
    // 1) ребра по весу
    std::vector<Edge> all_edges;

    for (int u = 0; u < this->n; u++) {
        for (auto& ver : graph[u]) {
            int v = ver.first;
            int weight = ver.second;

            if (u < v) {
                all_edges.push_back(Edge(u, v, weight));
            }
        }
    }
    std::sort(all_edges.begin(), all_edges.end());

    // 1) массивчик с вершинами
    std::vector<int> lables(this->n);
    std::iota(lables.begin(), lables.end(), 0);

    std::vector<Edge> tree;
    int total_weight = 0;
    int count_iter = 0;

    for (auto& edge : all_edges) {
        // 2) добавляем, если нет цикла
        if (lables[edge.u] != lables[edge.v]) {
            tree.push_back(edge);
            count_iter++;
            total_weight += edge.weight;

            int old_lable = lables[edge.v];
            int new_lable = lables[edge.u];

            // 3) апдейт компонент связанности 
            for (int i = 0; i < this->n; i++) {
                if (lables[i] == old_lable) {
                    lables[i] = new_lable;
                }
            }
            // 4) чек на конец пробежки
            if (count_iter == this->n - 1)
                break;
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto working_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    auto total_ms = working_time.count();
    long long minutes = total_ms / 60000;
    long long seconds = (total_ms % 60000) / 1000;
    long long milliseconds = total_ms % 1000;

    std::cout << "Kruskal total weight: " << total_weight << "\n";

    std::ofstream out("Kruskal.txt");
    if (!out.is_open()) {
        std::cout << "Error\n";
        return;
    }
    out << "Time working: " << minutes << "m " << seconds << "s " << milliseconds << "ms\n";

    for (auto& edge : tree) {
        out << "{ " << edge.u << ", " << edge.v << " } : " << edge.weight << "\n";
    }

    out.close();
}

void Graph::ListGraph::Prima() {
    auto start = std::chrono::high_resolution_clock::now();
    if (!IsConnected()) {
        std::cout << "Graph isnt connected\n";
        return;
    }

    bool* in_tree = new bool[this->n];
    for (int i = 0; i < this->n; i++){in_tree[i] = false; }
    std::vector<Edge> tree;

    // Вектор для хранения минимальных рёбер к каждой вершине
    std::vector<Edge> min_edges(this->n, Edge(-1, -1, std::numeric_limits<int>::max()));
    int total_weight = 0;

    // 1) Выбираем случайную начальную вершину
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, this->n - 1);

    int start_vertex = dist(gen);
    in_tree[start_vertex] = true;

    // 2) Инициализируем минимальные рёбра для соседей начальной вершины
    for (const auto& neighbor : graph[start_vertex]) {
        min_edges[neighbor.first] = Edge(start_vertex, neighbor.first, neighbor.second);
    }

    // 3) Пока не все вершины в дереве
    for (int tree_size = 1; tree_size < this->n; tree_size++) {
        Edge min_edge(-1, -1, std::numeric_limits<int>::max());

        // 4) Находим минимальное ребро к вершине не в дереве
        for (int v = 0; v < this->n; v++) {
            if (!in_tree[v] && min_edges[v].weight < min_edge.weight) {
                min_edge = min_edges[v];
            }
        }

        in_tree[min_edge.v] = true;
        total_weight += min_edge.weight;
        tree.push_back(min_edge);

        // 6) Обновляем минимальные рёбра для соседей новой вершины
        for (auto& neighbor : graph[min_edge.v]) {
            int& v = neighbor.first;
            int& weight = neighbor.second;

            if (!in_tree[v] && weight < min_edges[v].weight) {
                min_edges[v] = Edge(min_edge.v, v, weight);
            }
        }
    }
    delete[] in_tree;

    auto end = std::chrono::high_resolution_clock::now();
    auto working_time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Prima total weight: " << total_weight << "\n";

    std::ofstream out("Prima.txt");
    if (!out.is_open()) {
        std::cout << "Error\n";
        return;
    }

    auto total_ms = working_time.count();
    long long minutes = total_ms / 60000;
    long long seconds = (total_ms % 60000) / 1000;
    long long milliseconds = total_ms % 1000;

    out << "Time working: " << minutes << "m " << seconds << "s " << milliseconds << "ms\n";
    for (auto& edge : tree) {
        out << "{ " << edge.u << ", " << edge.v << " } : " << edge.weight << "\n";
    }
    out.close();
}

